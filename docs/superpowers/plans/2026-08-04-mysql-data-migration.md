# MySQL Data Migration Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace all data in the new `mydb` database with a normalized, verified snapshot of the stopped legacy `mydb` database.

**Architecture:** Use the new server's MySQL 8 container as the migration client. Back up the target, import the legacy snapshot into an isolated staging database, normalize the approved nullable values, then replace target rows inside one transaction while preserving the target schema.

**Tech Stack:** Docker 26, MySQL 8.0 client/server, `mysqldump`, SHA-256 verification, SSH.

## Global Constraints

- Source database: `47.105.40.164:3307/mydb`; confirmed stopped for writes.
- Target database: `47.119.128.174:3307/mydb` in container `blog-mysql`.
- Preserve the target schema created from the project's POJOs.
- Clear all current target rows after creating a restorable backup.
- Change legacy `user.banned IS NULL` to `false`.
- Delete legacy login records missing `username`, `ip`, or `time`.
- Assign deterministic IDs to remaining legacy login records whose IDs are null.
- Never print database passwords; read them only from root-readable property files.
- Keep the target backup, source snapshot, normalized data dump, and staging database until manual cleanup.

---

### Task 1: Capture Credentials and Back Up the Target

**Files:**
- Create remotely: `/root/blog-infra/source-db.properties`
- Create remotely: `/root/blog-infra/backups/20260804-112721-target-before-migration.sql`

**Interfaces:**
- Consumes: repository `HEAD` legacy datasource properties and `/root/blog-infra/credentials.properties`.
- Produces: a mode-600 target backup and root-only source credentials.

- [ ] Stream only the legacy datasource username and password from Git `HEAD` into `/root/blog-infra/source-db.properties` with mode `600`.
- [ ] Verify both source and target logins with `SELECT 1`.
- [ ] Run `mysqldump --single-transaction --quick --no-tablespaces --column-statistics=0 --set-gtid-purged=OFF mydb` against the target container.
- [ ] Confirm the backup is non-empty and passes a parser check by importing it into no database only with `mysql --force` prohibited; retain its SHA-256 digest.

### Task 2: Stage and Normalize the Legacy Snapshot

**Files:**
- Create remotely: `/root/blog-infra/backups/20260804-112721-source-snapshot.sql`
- Create remotely: `/root/blog-infra/backups/20260804-112721-normalized-data.sql`
- Create in MySQL: `mydb_migration_stage`

**Interfaces:**
- Consumes: legacy database and root-only credentials.
- Produces: normalized staging rows and a data-only SQL dump.

- [ ] Export the seven legacy tables with a single consistent transaction.
- [ ] Recreate `mydb_migration_stage` and import the legacy snapshot there.
- [ ] Verify pre-normalization counts: `answer=1`, `blog=3`, `evaluate=3`, `login_record=41`, `record_of_question=40`, `topic=3`, `user=6`.
- [ ] Execute `UPDATE user SET banned='false' WHERE banned IS NULL`.
- [ ] Execute `DELETE FROM login_record WHERE username IS NULL OR ip IS NULL OR time IS NULL`.
- [ ] Assign increasing IDs above the current maximum to remaining `login_record.id IS NULL` rows.
- [ ] Verify post-normalization counts: `answer=1`, `blog=3`, `evaluate=3`, `login_record=40`, `record_of_question=40`, `topic=3`, `user=6`, with no null cells required by the target schema.
- [ ] Export staging data only with complete column lists and no DDL or table locks.

### Task 3: Replace Target Rows Atomically

**Files:**
- Read remotely: `/root/blog-infra/backups/20260804-112721-normalized-data.sql`

**Interfaces:**
- Consumes: verified normalized data dump.
- Produces: replaced rows in target `mydb`, preserving its schema.

- [ ] Open one target MySQL session with `FOREIGN_KEY_CHECKS=0` and `START TRANSACTION`.
- [ ] Delete rows from all seven target tables.
- [ ] Import all normalized INSERT statements in the same session.
- [ ] Commit only if the client reaches the end without an SQL error; otherwise let connection close roll the transaction back.

### Task 4: Verify and Retain Rollback Artifacts

**Files:**
- Retain remotely: all three SQL files under `/root/blog-infra/backups/`.

**Interfaces:**
- Consumes: staging and target databases.
- Produces: row-count and content-hash evidence.

- [ ] Compare exact row counts between `mydb_migration_stage` and `mydb` for all seven tables.
- [ ] Compare SHA-256 hashes of `SELECT * FROM <table> ORDER BY id` output for all seven tables.
- [ ] Verify the application database account can read `mydb`.
- [ ] Verify target schema still has seven InnoDB tables using `utf8mb4_unicode_ci`.
- [ ] Verify `user.banned` has no nulls and no login record lacks username, IP, or time.
- [ ] Delete `/root/blog-infra/source-db.properties`; retain SQL backups and staging database for rollback.

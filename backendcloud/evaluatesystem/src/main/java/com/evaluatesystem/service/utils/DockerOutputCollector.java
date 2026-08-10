package com.evaluatesystem.service.utils;

import com.github.dockerjava.core.command.ExecStartResultCallback;

import java.io.ByteArrayOutputStream;
import java.nio.charset.StandardCharsets;

final class DockerOutputCollector extends ExecStartResultCallback {
    private final ByteArrayOutputStream stdout;
    private final ByteArrayOutputStream stderr;

    DockerOutputCollector() {
        this(new ByteArrayOutputStream(), new ByteArrayOutputStream());
    }

    private DockerOutputCollector(ByteArrayOutputStream stdout, ByteArrayOutputStream stderr) {
        super(stdout, stderr);
        this.stdout = stdout;
        this.stderr = stderr;
    }

    String getStdout() {
        return new String(stdout.toByteArray(), StandardCharsets.UTF_8);
    }

    String getStderr() {
        return new String(stderr.toByteArray(), StandardCharsets.UTF_8);
    }
}

package com.evaluatesystem.service.utils;

import com.github.dockerjava.api.model.Frame;
import com.github.dockerjava.api.model.StreamType;
import org.junit.jupiter.api.Test;

import java.nio.charset.StandardCharsets;

import static org.junit.jupiter.api.Assertions.assertEquals;

class DockerOutputCollectorTests {

    @Test
    void accumulatesStdoutAcrossDockerFrames() {
        DockerOutputCollector collector = new DockerOutputCollector();

        collector.onNext(new Frame(StreamType.STDOUT, "3".getBytes(StandardCharsets.UTF_8)));
        collector.onNext(new Frame(StreamType.STDOUT, "\n".getBytes(StandardCharsets.UTF_8)));

        assertEquals("3\n", collector.getStdout());
    }
}

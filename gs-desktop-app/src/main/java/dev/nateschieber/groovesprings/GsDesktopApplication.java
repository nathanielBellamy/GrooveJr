package dev.nateschieber.groovesprings;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.Configuration;

@SpringBootApplication
@Configuration
public class GsDesktopApplication {

  public static void main(String[]  args) {
    System.out.println("======= GS DESKTOP APPLICATION ");
    SpringApplication.run(GsDesktopApplication.class, args);
  }
}

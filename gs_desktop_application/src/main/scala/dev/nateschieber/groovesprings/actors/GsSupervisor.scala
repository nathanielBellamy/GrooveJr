package dev.nateschieber.groovesprings.actors

import akka.actor.typed.Behavior
import akka.actor.typed.PostStop
import akka.actor.typed.Signal
import akka.actor.typed.scaladsl.AbstractBehavior
import akka.actor.typed.scaladsl.ActorContext
import akka.actor.typed.scaladsl.Behaviors
import dev.nateschieber.groovesprings.traits.InitDisplay

object GsSupervisor {
  def apply(): Behavior[Nothing] = Behaviors.setup {
    context =>
      val playbackRef = context.spawn(GsPlayback(), "gs_playback")
      val displayRef = context.spawn(GsDisplay(), "gs_display")
      
      displayRef ! InitDisplay(playbackRef)
      
      new GsSupervisor(context)
  }
}

class GsSupervisor(context: ActorContext[Nothing]) extends AbstractBehavior[Nothing](context) {
  println("gs_desktop_application started")

  override def onMessage(msg: Nothing): Behavior[Nothing] = {
    Behaviors.unhandled
  }

  override def onSignal: PartialFunction[Signal, Behavior[Nothing]] = {
    case PostStop =>
      println("gs_desktop_application stopped")
      this
  }
}
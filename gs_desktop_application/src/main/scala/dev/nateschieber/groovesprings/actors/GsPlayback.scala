package dev.nateschieber.groovesprings.actors

import akka.actor.typed.ActorRef
import akka.actor.typed.Behavior
import akka.actor.typed.PostStop
import akka.actor.typed.Signal
import akka.actor.typed.receptionist.{Receptionist, ServiceKey}
import akka.actor.typed.scaladsl.AbstractBehavior
import akka.actor.typed.scaladsl.ActorContext
import akka.actor.typed.scaladsl.Behaviors
import dev.nateschieber.groovesprings.jni.JniMain
import dev.nateschieber.groovesprings.traits.*

object GsPlayback {

  val GsPlaybackServiceKey = ServiceKey[GsCommand]("gs_playback")

  def apply(): Behavior[GsCommand] = Behaviors.setup {
    context =>
      context.system.receptionist ! Receptionist.Register(GsPlaybackServiceKey, context.self)
      new GsPlayback(context)
  }
}

class GsPlayback(context: ActorContext[GsCommand]) extends AbstractBehavior[GsCommand](context) {

  private var playbackThreadRef: ActorRef[GsCommand] = null

  override def onMessage(msg: GsCommand): Behavior[GsCommand] = {
    msg match {
      case ReadFrameId(replyTo) =>
        replyTo ! RespondFrameId(GsPlaybackThread.currFrameId, context.self)
        Behaviors.same

      case PlayTrig(replyTo) =>
        println("GsPlayback :: play")
        playbackThreadRef = context.spawn(GsPlaybackThread(), "gs_playback_thread")
        playbackThreadRef ! InitPlaybackThread(context.self)
        replyTo ! RespondPlayTrig(context.self)
        Behaviors.same
      
      case PauseTrig(replyTo) =>
        println("GsPlayback :: pause")
        replyTo ! RespondPauseTrig(context.self)
        Behaviors.same

      case StopTrig(replyTo) =>
        println("GsPlayback :: stop")
        if (playbackThreadRef != null) {
          playbackThreadRef ! StopPlaybackThread(context.self)
          playbackThreadRef = null
        }
        replyTo ! RespondStopTrig(context.self)
        Behaviors.same
        
      case FastForwardTrig(replyTo) =>
        println("GsPlayback :: fastForward")
        replyTo ! RespondFastForwardTrig(context.self)
        Behaviors.same

      case RewindTrig(replyTo) =>
        println("GsPlayback :: rewind")
        replyTo ! RespondRewindTrig(context.self)
        Behaviors.same
    }
  }

  override def onSignal: PartialFunction[Signal, Behavior[GsCommand]] = {
    case PostStop =>
      this
  }
}

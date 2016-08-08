/*
 * Playback file
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Geom/Playback.h>


namespace Gm
{


/* ----- Default playback event listeners ----- */

void Playback::OneShot::OnNextFrame(Playback& sender)
{
    if (sender.HasEndReached())
        sender.Stop();
    else
        sender.SetNextFrame();
}

void Playback::Loop::OnNextFrame(Playback& sender)
{
    if (sender.HasEndReached())
        sender.SetNextFrame(sender.IsForward() ? sender.firstFrame : sender.lastFrame);
    else
        sender.SetNextFrame();
}

void Playback::PingPongLoop::OnNextFrame(Playback& sender)
{
    if (sender.HasEndReached())
        std::swap(sender.firstFrame, sender.lastFrame);
    sender.SetNextFrame();
}

void Playback::ListLoop::OnStop(Playback& sender)
{
    iterator = 0;
}

void Playback::ListLoop::OnNextFrame(Playback& sender)
{
    auto count = frames.size();

    if (count >= 2)
    {
        if (sender.IsForward())
        {
            if (iterator >= (count - 1))
                iterator = 0;
            else
                ++iterator;
        }
        else
        {
            if (iterator == 0)
                iterator = (count - 1);
            else
                --iterator;
        }

        sender.SetNextFrame(frames[iterator]);
    }
}


/* ----- Playback class ----- */

Playback::Playback() :
    eventListener_( std::make_shared<Playback::OneShot>() )
{
}

void Playback::Play(FrameIndex firstFrameIndex, FrameIndex lastFrameIndex, Gs::Real playbackSpeed, const std::shared_ptr<EventListener>& eventListener)
{
    /* Stop previous playback */
    Stop();

    /* Setup event listener */
    eventListener_  = (eventListener != nullptr ? eventListener : std::make_shared<Playback::OneShot>());

    /* Setup frame indices */
    firstFrame      = firstFrameIndex;
    lastFrame       = lastFrameIndex;
    frame           = firstFrame;

    /* Setup interpolator and speed */
    interpolator    = Gs::Real(0);
    speed           = playbackSpeed;

    /* Start playing and acquire next frame for the first time */
    SetState(State::Playing);
    PostNextFrame();
}

void Playback::Play(FrameIndex firstFrameIndex, FrameIndex lastFrameIndex, const std::shared_ptr<EventListener>& eventListener)
{
    Play(firstFrameIndex, lastFrameIndex, speed, eventListener);
}

void Playback::Play(const std::shared_ptr<EventListener>& eventListener)
{
    Play(firstFrame, lastFrame, speed, eventListener);
}

void Playback::Pause(bool paused)
{
    if (paused)
    {
        /* Pause playback */
        if (GetState() == State::Playing)
            SetState(State::Paused);
    }
    else
    {
        /* Resume playback */
        if (GetState() == State::Paused)
            SetState(State::Playing);
    }
}

void Playback::Stop()
{
    if (GetState() != State::Stopped)
        SetState(State::Stopped);
}

void Playback::Update(Gs::Real deltaTime)
{
    /* Check if playback is currently being played */
    if (GetState() != State::Playing || deltaTime <= Gs::Real(0))
        return;

    /* Increase frame interpolator */
    interpolator += (deltaTime * speed);

    if (IsForward())
    {
        /* Post event for each animation frame */
        while (interpolator >= Gs::Real(1))
        {
            /* Set current frame to the next frame */
            interpolator -= Gs::Real(1);
            frame = nextFrame;

            /* Get next frame by event listener */
            PostNextFrame();
        }
    }
    else
    {
        /* Post event for each animation frame */
        while (interpolator <= Gs::Real(0))
        {
            /* Set current frame to the next frame */
            interpolator += Gs::Real(1);
            nextFrame = frame;

            /* Get next frame by event listener */
            PostNextFrame();
        }
    }
}

void Playback::SetNextFrame(FrameIndex nextFrameIndex)
{
    if (IsForward())
        nextFrame = nextFrameIndex;
    else
        frame = nextFrameIndex;
}

void Playback::SetNextFrame()
{
    /* Set next frame index depending on playback chronology */
    if (IsForward())
        nextFrame = (AreFramesChrono() ? frame + 1 : frame - 1);
    else
        frame = (AreFramesChrono() ? frame - 1 : frame + 1);
}

bool Playback::HasEndReached() const
{
    if (IsForward())
        return (AreFramesChrono() ? frame >= lastFrame : frame <= lastFrame);
    else
        return (AreFramesChrono() ? frame <= firstFrame : frame >= firstFrame);
}


/*
 * ======= Private: =======
 */

void Playback::SetState(const State state)
{
    /* Set new playback state */
    state_ = state;

    /* Post respective state event */
    switch (state)
    {
        case State::Playing:
            eventListener_->OnPlay(*this);
            break;
        case State::Paused:
            eventListener_->OnPause(*this);
            break;
        case State::Stopped:
            eventListener_->OnStop(*this);
            break;
    }
}

void Playback::PostNextFrame()
{
    eventListener_->OnNextFrame(*this);
}


} // /namespace Gm



// ================================================================================

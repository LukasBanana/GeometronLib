/*
 * Playback.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_PLAYBACK_H
#define GM_PLAYBACK_H


#include <Gauss/Real.h>
#include <memory>
#include <vector>


namespace Gm
{


/**
\brief Animation playback class.
\remarks This class does not store any data about the keyframes or transformations.
Only the process of playing an animation is managed by this class.
Although this class has an event listener, there are no "Post..." functions,
because the events are posted from the internal functions (such as "Update", "Play" etc.).
No matter what direction the playback moves, an interpolation between frames is allways computed as follows:
\code
auto from   = playback.frame;           // Frame index 'from' which we interpolate.
auto to     = playback.nextFrame;       // Frame index 'to' which we interpolate.
auto time   = playback.interpolator;    // Interpolation time (in the range [0.0 .. 1.0]).
auto result = Gs::Lerp(MyTransformations[from], MyTransformations[to], time);
\endcode
*/
class Playback
{

    public:
        
        //! Type of the frame indices (unsigned integral type).
        using FrameIndex = std::size_t;

        //! Playback state enumeration.
        enum class State
        {
            Playing,    //!< Animation is currently playing.
            Paused,     //!< Animation has been paused.
            Stopped,    //!< Animation has been stopped.
        };

        //! Playback event listener interface.
        class EventListener
        {

            public:

                virtual ~EventListener()
                {
                }

                /**
                \brief Receives the 'playback start' event. All playback configurations are done when this event is posted.
                \see Playback::Play
                */
                virtual void OnPlay(Playback& sender)
                {
                }

                /**
                \brief Receives the 'playback paused' event. All playback configurations are done when this event is posted.
                \see Playback::Pause
                */
                virtual void OnPause(Playback& sender)
                {
                }

                /**
                \breif Receives the 'playback stopped' event. All playback configurations are done when this event is posted.
                \remarks This will only be posted if the playback was previously being played or paused.
                \see Playback::Stop
                */
                virtual void OnStop(Playback& sender)
                {
                }

                /**
                \breif Receives the 'next frame' event. This event will be posted in the "Playback::Update" function.
                \remarks Depending on the playback speed sometimes this event will be posted several times for
                a single call to the "Playback::Update" function. This happens when several frames will be skipped,
                i.e. every single frame can be examined with this event listener interface, no matter how fast the playback speed is.
                All playback configurations for the next frame are done when this event is posted.
                This function should set the next frame (see 'nextFrame' field).
                \see Playback::Update
                \see Playback::nextFrame
                */
                virtual void OnNextFrame(Playback& sender)
                {
                }
                
        };

        /**
        \brief One shot playback event listener.
        \remarks Plays an animation from the first to the last frame.
        */
        class OneShot : public EventListener
        {

            public:
                
                void OnNextFrame(Playback& sender) override;

        };

        /**
        \brief Loop playback event listener.
        \remarks Plays an animation from the first to the last frame, and then starts the animation from the beginning.
        */
        class Loop : public EventListener
        {

            public:
                
                void OnNextFrame(Playback& sender) override;

        };

        /**
        \brief Ping-pong loop playback event listener.
        \remarks Plays an animation from the first to the last frame, and vise-versa, and then starts the animation from the beginning.
        */
        class PingPongLoop : public EventListener
        {

            public:
                
                void OnNextFrame(Playback& sender) override;

        };

        /**
        \brief List loop playback event listener.
        \remarks Plays an animation by the listed frame indices in a loop fashion.
        */
        class ListLoop : public EventListener
        {

            public:
                
                void OnStop(Playback& sender) override;
                void OnNextFrame(Playback& sender) override;

                std::vector<FrameIndex>             frames;         //!< Animation frame indices.
                std::vector<FrameIndex>::size_type  iterator = 0;   //!< Iteration index for the animation frames.

        };

        Playback();

        /**
        \brief Starts the playback process.
        \param[in] firstFrameIndex Specifies the index of the first frame. This may also be greater than the index of the last frame.
        \param[in] lastFrameIndex Specifies the index of the last frame. This may also be less than the index of the first frame.
        \param[in] newSpeed Specifies the animation speed factor. This may also be negative (to animate backward). By default 1.0.
        \param[in] eventListener Shared pointer to the event listener. If this is null, the "OneShot" event listener will be used as default. By default null.
        */
        void Play(FrameIndex firstFrameIndex, FrameIndex lastFrameIndex, Gs::Real playbackSpeed, const std::shared_ptr<EventListener>& eventListener = nullptr);

        /**
        \brief Starts the playback process with the previous speed.
        \see Play(FrameIndex, FrameIndex, Gs::Real, const std::shared_ptr<EventListener>&)
        */
        void Play(FrameIndex firstFrameIndex, FrameIndex lastFrameIndex, const std::shared_ptr<EventListener>& eventListener = nullptr);

        /**
        \brief Starts the playback process with the previous frame indicies and speed.
        \see Play(FrameIndex, FrameIndex, Gs::Real, const std::shared_ptr<EventListener>&)
        */
        void Play(const std::shared_ptr<EventListener>& eventListener = nullptr);

        /**
        \brief Pauses or resumes the animation playback.
        \param[in] paused Specifies whether the playback is to be paused or resumed.
        If true the playback will be paused otherwise it will be resumed.
        An animation playback can only be resumed if it was paused previously (state must be 'State::Paused').
        An animation playback can only be paused if it was playing previously (state must be 'State::Playing').
        \see GetState
        \see State
        */
        void Pause(bool paused = true);

        /**
        \brief Stops the animation playback. After this call the state will be State::Stopped.
        \see GetState
        \see State
        */
        void Stop();

        /**
        \brief Updates the playback process. This increases (or decreases if speed is negative) the frame interpolator.
        \param[in] deltaTime Specifies the time derivation between the previous and current frame.
        If the application runs with 60 Hz this value should be 1.0/60.0, if the application runs with 200 Hz
        it should be 1.0/200.0 etc. This value will be added (and always added, also when the playback is backward)
        to the 'interpolator'.
        \remarks If the frame interpolator is greater than or equal to 1.0 the next frame will be set.
        Also the "OnNextFrame" function will be called from the event listener (see EventListener class).
        If the animation is currently not playing (state must be 'State::Playing') or
        'deltaTime' is less than or equal to 0.0, this function call has no effect.
        \see EventListener
        \see Play
        */
        void Update(Gs::Real deltaTime);

        /**
        \brief Sets the next frame depending on the playback direction.
        \param[in] nextFrameIndex Specifies the next frame index.
        \remarks If the playback direction is forward the field 'nextFrame' is set, otherwise the field 'frame' is set.
        \see IsForward
        */
        void SetNextFrame(FrameIndex nextFrameIndex);

        /**
        \brief Sets the next frame depending on the current frame, the playback direction, and the frame chronlogy.
        \remarks The frame index range (first and last frame indices) is ignored here.
        \see IsForward
        \see AreFramesChrono
        */
        void SetNextFrame();

        /**
        \breif Returns true if the end of animation playback has been reached.
        \remarks This depends on the playback direction and chronology.
        \see IsForward
        \see AreFramesChrono
        */
        bool HasEndReached() const;

        //! Returns the playback state.
        inline State GetState() const
        {
            return state_;
        }

        /**
        \brief Returns true if the first- and last frames are chronologic.
        \remarks This is the case when the first frame is less than or equal to the last frame (firstFrame <= lastFrame).
        \see firstFrame
        \see lastFrame
        */
        inline bool AreFramesChrono() const
        {
            return (firstFrame <= lastFrame);
        }

        /**
        \brief Returns true if the playback moves frowards.
        \remarks This is the case when the speed is non-negative (speed >= 0.0).
        \see speed
        */
        inline bool IsForward() const
        {
            return (speed >= 0.0);
        }

        /* === Members === */

        //! First frame index, in the range [0, +inf). By default 0.
        FrameIndex  firstFrame      = 0;

        //! Last frame index, in the range [0, +inf). By default 0.
        FrameIndex  lastFrame       = 0;

        //! Current frame index, in the range [firstFrame, lastFrame]. By default 0.
        FrameIndex  frame           = 0;

        //! Next frame index, in the range [firstFrame, lastFrame]. By default 0.
        FrameIndex  nextFrame       = 0;

        //! Frame interpolator, in the range [0.0, 1.0]. By default 0.
        Gs::Real    interpolator    = Gs::Real(0);

        //! Animation speed factor, in the range (-inf, +inf). By default 1.
        Gs::Real    speed           = Gs::Real(1);

    private:
        
        void SetState(const State state);
        void PostNextFrame();

        State                           state_ = State::Stopped;

        std::shared_ptr<EventListener>  eventListener_;

};


} // /namespace Gm


#endif



// ================================================================================

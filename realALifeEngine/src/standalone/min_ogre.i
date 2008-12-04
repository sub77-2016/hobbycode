// Minimal WRAPPING OF OGRE
//%include "OGRE/OgreConfig.h"
%include "OGRE/OgrePlatform.h"
//%include "OGRE/OgrePrerequisites.h"
//%include "OGRE/Ogre.h"

namespace Ogre {
    // define the real number values to be used
    // default to use 'float' unless precompiler option set
    #if OGRE_DOUBLE_PRECISION == 1
		/** Software floating point type.
		@note Not valid as a pointer to GPU buffers / parameters
		*/
        typedef double Real;
    #else
		/** Software floating point type.
		@note Not valid as a pointer to GPU buffers / parameters
		*/
        typedef float Real;
    #endif
    
        /** Struct containing information about a frame event.
    */
    struct FrameEvent
    {
        /** Elapsed time in seconds since the last event.
            This gives you time between frame start & frame end,
            and between frame end and next frame start.
            @remarks
                This may not be the elapsed time but the average
                elapsed time between recently fired events.
        */
        Real timeSinceLastEvent;
        /** Elapsed time in seconds since the last event of the same type,
            i.e. time for a complete frame.
            @remarks
                This may not be the elapsed time but the average
                elapsed time between recently fired events of the same type.
        */
        Real timeSinceLastFrame;
    };


    /** A interface class defining a listener which can be used to receive
        notifications of frame events.
        @remarks
            A 'listener' is an interface designed to be called back when
            particular events are called. This class defines the
            interface relating to frame events. In order to receive
            notifications of frame events, you should create a subclass of
            FrameListener and override the methods for which you would like
            to customise the resulting processing. You should then call
            Root::addFrameListener passing an instance of this class.
            There is no limit to the number of frame listeners you can register,
            allowing you to register multiple listeners for different purposes.
            Frame events only occur when Ogre is in continuous rendering mode,
            ie. after Root::startRendering is called. If the application is
            doing ad-hoc rendering without entering a rendering loop, frame
            events are not generated. Note that a frame event occurs once for
            all rendering targets, not once per target.
    */
    class _OgreExport FrameListener
    {
        /*
        Note that this could have been an abstract class, but I made
        the explicit choice not to do this, because I wanted to give
        people the option of only implementing the methods they wanted,
        rather than having to create 'do nothing' implementations for
        those they weren't interested in. As such this class follows
        the 'Adapter' classes in Java rather than pure interfaces.
        */
    public:
        /** Called when a frame is about to begin rendering.
            @return
                True to go ahead, false to abort rendering and drop
                out of the rendering loop.
        */
        virtual bool frameStarted(const FrameEvent& evt) { return true; }
        /** Called just after a frame has been rendered.
            @return
                True to continue with the next frame, false to drop
                out of the rendering loop.
        */
        virtual bool frameEnded(const FrameEvent& evt) { return true; }

		virtual ~FrameListener() {}
		
    };

}

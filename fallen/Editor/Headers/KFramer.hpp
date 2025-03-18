// KFramer.hpp
// Guy Simmons, 19th September 1997.

#ifndef _KFRAMER_HPP_
#define _KFRAMER_HPP_

#include "EditMod.hpp"

#define CTRL_KF_LOAD_BUTTON 1
#define CTRL_KF_FRAME_SLIDER 2

class KeyFrameEditor2 : public EditorModule {
   private:
    std::int32_t AnimAngleX,
        AnimAngleY,
        AnimAngleZ,
        AnimOffsetX,
        AnimOffsetY,
        AnimOffsetZ,
        AnimScale,
        AnimCount,
        AnimTween;
    ControlSet KeyFramesControls;
    EdRect KeyFramesRect;

   public:
    ~KeyFrameEditor2();
    void SetupModule();
    void DrawContent();
    void HandleContentClick(std::uint8_t flags, MFPoint *clicked_point);
    void HandleControlClick(std::uint8_t flags, MFPoint *clicked_point);
    void HandleModule();
    void HandleKeyFramesControl(std::uint32_t control_id);
    void DrawKeyFrames();
    void DrawKeyFrame(std::uint16_t multi_object, EdRect *bounds_rect, struct KeyFrame *the_frame, struct Matrix33 *r_matrix);
};

#endif

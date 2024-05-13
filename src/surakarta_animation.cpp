#include <malloc.h>
#include <cmath>
#include <memory>
#include <vector>
#include "surakarta_piece.h"

class SurakartaAnimationImpl : public SurakartaAnimationBase {
   public:
    SurakartaAnimationImpl(
        const std::vector<SurakartaMovePathFragment>& trace,
        int total_milliseconds = 1000);

    virtual ~SurakartaAnimationImpl() override;

    // Each call should have milliseconds no less than the previous call.
    virtual Point PositionAt(int milliseconds) override;

    virtual bool Finished() const override {
        return index_ == total_;
    }

   private:
    int total_;
    SurakartaMovePathFragment* trace_;
    int* length_;
    int total_length_;  // ideal * 1000
    int* time_;
    int* start_time_;
    int index_;
    bool delta_cached;
    double dx, dy, dtheta;
};

std::shared_ptr<SurakartaAnimationBase> SurakartaMoveTrace::ToAnimation(int total_milliseconds) const {
    return std::make_shared<SurakartaAnimationImpl>(path, total_milliseconds);
}

constexpr int ideal = 1000;
constexpr int root2 = 1414;
constexpr int pi3d2 = 4712;

#define CHOOSE(FRAGMENT, IF_CURVE, IF_STRAIGHT, IF_SLASH) (                                                                                           \
    (FRAGMENT).is_curve                                                                                                                               \
        ? (IF_CURVE)                                                                                                                                  \
        : (((FRAGMENT).info.straight.start_x == (FRAGMENT).info.straight.end_x || (FRAGMENT).info.straight.start_y == (FRAGMENT).info.straight.end_y) \
               ? (IF_STRAIGHT)                                                                                                                        \
               : (IF_SLASH)))

#define LENGTH(FRAGMENT) CHOOSE(FRAGMENT, pi3d2*(FRAGMENT).info.curve.radius, ideal, root2)

SurakartaAnimationImpl::SurakartaAnimationImpl(
    const std::vector<SurakartaMovePathFragment>& trace,
    int total_milliseconds) {
    total_ = trace.size();
    void* memory = malloc((sizeof(SurakartaMovePathFragment) + sizeof(int) * 2) * total_ + sizeof(int) * (total_ + 2));
    trace_ = (SurakartaMovePathFragment*)memory;
    length_ = (int*)(&trace_[total_]);
    time_ = (int*)(&length_[total_]);
    start_time_ = (int*)(&time_[total_]);
    total_length_ = 0;
    for (int i = 0; i < total_; i++) {
        trace_[i] = trace[i];
        length_[i] = LENGTH(trace_[i]);
        total_length_ += length_[i];
    }
    const int straight_time = (double)total_milliseconds / total_length_ * ideal;
    const int slash_time = (double)total_milliseconds / total_length_ * root2;
    const int curve_time = (double)total_milliseconds / total_length_ * pi3d2;
    for (int i = 0; i < total_; i++)
        time_[i] = CHOOSE(trace_[i], curve_time * trace_[i].info.curve.radius, straight_time, slash_time);
    start_time_[0] = 0;
    for (int i = 1; i < total_; i++) {
        start_time_[i] = start_time_[i - 1] + time_[i - 1];
    }
    start_time_[total_] = total_milliseconds;
    start_time_[total_] = total_milliseconds;
    index_ = 0;
    delta_cached = false;
}

SurakartaAnimationImpl::~SurakartaAnimationImpl() {
    if (trace_)
        free(trace_);
    // One free is enough.
}

SurakartaAnimationImpl::Point SurakartaAnimationImpl::PositionAt(int milliseconds) {
    while (milliseconds >= start_time_[index_ + 1]) {
        delta_cached = false;
        if (index_ < total_)
            index_++;
        else {
            const auto to = trace_[total_ - 1].To();
            Point ret{
                .x = static_cast<double>(to.x),
                .y = static_cast<double>(to.y)};
            return ret;
        }
    }
    if (trace_[index_].is_curve) {
        const int r = trace_[index_].info.curve.radius;
        if (delta_cached == false) {
            if (trace_[index_].info.curve.is_clockwise)
                dtheta = -1.0 / time_[index_] * M_PI_2 * 3;
            else
                dtheta = +1.0 / time_[index_] * M_PI_2 * 3;
            delta_cached = true;
        }
        const double theta = trace_[index_].info.curve.start_angle * M_PI_2 + (double)(milliseconds - start_time_[index_]) * dtheta;
        Point ret{
            .x = trace_[index_].info.curve.center_x + r * cos(theta),
            .y = trace_[index_].info.curve.center_y + r * sin(theta),
        };
        return ret;
    } else {
        if (delta_cached == false) {
            dx = (double)(trace_[index_].info.straight.end_x - trace_[index_].info.straight.start_x) / time_[index_];
            dy = (double)(trace_[index_].info.straight.end_y - trace_[index_].info.straight.start_y) / time_[index_];
            delta_cached = true;
        }
        double x = trace_[index_].info.straight.start_x + (milliseconds - start_time_[index_]) * dx;
        double y = trace_[index_].info.straight.start_y + (milliseconds - start_time_[index_]) * dy;
        Point ret{
            .x = x,
            .y = y};
        return ret;
    }
}

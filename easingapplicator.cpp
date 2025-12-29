#include "easingapplicator.h"

EasingApplicator::EasingApplicator()
    : easingType(EasingType::Linear)
    , easingMode(EasingMode::InOut)
{
}

void EasingApplicator::setEasingType(EasingType type)
{
    easingType = type;
}

void EasingApplicator::setEasingSelect(int index)
{
    // Map index to easing type
    if (index >= 0 && index < 13) {
        easingType = static_cast<EasingType>(index);
    } else {
        easingType = EasingType::Linear;
    }

    // Set easingMode based on easingType
    switch (easingType) {
        case EasingType::QuadIn:
        case EasingType::CubicIn:
        case EasingType::SineIn:
            easingMode = EasingMode::In;
            break;
        case EasingType::QuadOut:
        case EasingType::CubicOut:
        case EasingType::SineOut:
            easingMode = EasingMode::Out;
            break;
        case EasingType::QuadInOut:
        case EasingType::CubicInOut:
        case EasingType::SineInOut:
        case EasingType::Elastic:
        case EasingType::Bounce:
        case EasingType::Back:
            easingMode = EasingMode::InOut;
            break;
        default:
            easingMode = EasingMode::InOut;
            break;
    }
}

void EasingApplicator::setEasingMode(EasingMode mode)
{
    easingMode = mode;
}

double EasingApplicator::easeLinear(double t) const
{
    return t;
}

double EasingApplicator::easeQuad(double t) const
{
    switch (easingMode) {
        case EasingMode::In:
            return t * t;
        case EasingMode::Out:
            return 1.0 - (1.0 - t) * (1.0 - t);
        case EasingMode::InOut:
            if (t < 0.5) {
                return 2.0 * t * t;
            } else {
                return 1.0 - std::pow(-2.0 * t + 2.0, 2.0) / 2.0;
            }
        default:
            return t;
    }
}

double EasingApplicator::easeCubic(double t) const
{
    switch (easingMode) {
        case EasingMode::In:
            return t * t * t;
        case EasingMode::Out:
            return 1.0 - std::pow(1.0 - t, 3.0);
        case EasingMode::InOut:
            if (t < 0.5) {
                return 4.0 * t * t * t;
            } else {
                return 1.0 - std::pow(-2.0 * t + 2.0, 3.0) / 2.0;
            }
        default:
            return t;
    }
}

double EasingApplicator::easeSine(double t) const
{
    switch (easingMode) {
        case EasingMode::In:
            return 1.0 - std::cos((t * M_PI) / 2.0);
        case EasingMode::Out:
            return std::sin((t * M_PI) / 2.0);
        case EasingMode::InOut:
            return -(std::cos(M_PI * t) - 1.0) / 2.0;
        default:
            return t;
    }
}

double EasingApplicator::easeElastic(double t) const
{
    const double c4 = (2.0 * M_PI) / 3.0;

    if (t == 0.0) return 0.0;
    if (t == 1.0) return 1.0;

    switch (easingMode) {
        case EasingMode::In:
            return -std::pow(2.0, 10.0 * t - 10.0) * std::sin((t * 10.0 - 10.75) * c4);
        case EasingMode::Out:
            return std::pow(2.0, -10.0 * t) * std::sin((t * 10.0 - 0.75) * c4) + 1.0;
        case EasingMode::InOut:
            if (t < 0.5) {
                return -(std::pow(2.0, 20.0 * t - 10.0) * std::sin((20.0 * t - 11.125) * c4)) / 2.0;
            } else {
                return (std::pow(2.0, -20.0 * t + 10.0) * std::sin((20.0 * t - 11.125) * c4)) / 2.0 + 1.0;
            }
        default:
            return t;
    }
}

double EasingApplicator::easeBounce(double t) const
{
    const double n1 = 7.5625;
    const double d1 = 2.75;

    auto bounceOut = [n1, d1](double x) -> double {
        if (x < 1.0 / d1) {
            return n1 * x * x;
        } else if (x < 2.0 / d1) {
            x -= 1.5 / d1;
            return n1 * x * x + 0.75;
        } else if (x < 2.5 / d1) {
            x -= 2.25 / d1;
            return n1 * x * x + 0.9375;
        } else {
            x -= 2.625 / d1;
            return n1 * x * x + 0.984375;
        }
    };

    switch (easingMode) {
        case EasingMode::In:
            return 1.0 - bounceOut(1.0 - t);
        case EasingMode::Out:
            return bounceOut(t);
        case EasingMode::InOut:
            if (t < 0.5) {
                return (1.0 - bounceOut(1.0 - 2.0 * t)) / 2.0;
            } else {
                return (1.0 + bounceOut(2.0 * t - 1.0)) / 2.0;
            }
        default:
            return t;
    }
}

double EasingApplicator::easeBack(double t) const
{
    const double c1 = 1.70158;
    const double c2 = c1 * 1.525;
    const double c3 = c1 + 1.0;

    switch (easingMode) {
        case EasingMode::In:
            return c3 * t * t * t - c1 * t * t;
        case EasingMode::Out:
            return 1.0 + c3 * std::pow(t - 1.0, 3.0) + c1 * std::pow(t - 1.0, 2.0);
        case EasingMode::InOut:
            if (t < 0.5) {
                return (std::pow(2.0 * t, 2.0) * ((c2 + 1.0) * 2.0 * t - c2)) / 2.0;
            } else {
                return (std::pow(2.0 * t - 2.0, 2.0) * ((c2 + 1.0) * (t * 2.0 - 2.0) + c2) + 2.0) / 2.0;
            }
        default:
            return t;
    }
}

double EasingApplicator::applyEasing(double t) const
{
    // Clamp to [0, 1]
    t = std::clamp(t, 0.0, 1.0);

    switch (easingType) {
        case EasingType::Linear:
            return easeLinear(t);
        case EasingType::QuadIn:
        case EasingType::QuadOut:
        case EasingType::QuadInOut:
            return easeQuad(t);
        case EasingType::CubicIn:
        case EasingType::CubicOut:
        case EasingType::CubicInOut:
            return easeCubic(t);
        case EasingType::SineIn:
        case EasingType::SineOut:
        case EasingType::SineInOut:
            return easeSine(t);
        case EasingType::Elastic:
            return easeElastic(t);
        case EasingType::Bounce:
            return easeBounce(t);
        case EasingType::Back:
            return easeBack(t);
        default:
            return t;
    }
}

double EasingApplicator::process(double startValue, double endValue, double progress)
{
    // Apply easing function to progress
    double easedProgress = applyEasing(progress);

    // Interpolate between start and end
    double output = startValue + (endValue - startValue) * easedProgress;

    return output;
}

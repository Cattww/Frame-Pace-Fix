#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/CCParticleSystem.hpp>

using namespace geode::prelude;

/* * RobTop's physics step is hardcoded to 60fps logic.
 * This mod clamps the delta and smooths it to prevent
 * clipping on 720hz monitors and heavy startpos lag.
 */
#define PHYSICAL_DT_LIMIT 0.05f
constexpr float target_720_dt = 1.f / 720.f;

// global stutter state for the particle hook
int g_stutter_grace = 0;
bool g_is_lagging_bad = false;

class $modify(PlayLayer) {
    struct Fields {
        float smth_dt = target_720_dt;
        bool fresh_respawn = true;
    };

    bool init(GJGameLevel * p0, bool p1, bool p2) {
        if (!PlayLayer::init(p0, p1, p2)) return false;

        m_fields->smth_dt = target_720_dt;
        m_fields->fresh_respawn = true;
        g_stutter_grace = 0;

        // log::debug("PlayLayer: physics pacer online");
        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        m_fields->fresh_respawn = true;
        g_stutter_grace = 0;
    }

    void update(float dt) {
        // if a frame takes more than 25ms, the engine is dying.
        if (dt > 0.025f) {
            g_stutter_grace = 60; // skip particles for 60 frames
            g_is_lagging_bad = true;
        }

        float safe_dt = dt;
        if (safe_dt > PHYSICAL_DT_LIMIT) safe_dt = PHYSICAL_DT_LIMIT;

        if (m_fields->fresh_respawn) {
            m_fields->smth_dt = safe_dt;
            m_fields->fresh_respawn = false;
        }
        else {
            // fast ramp-up for high refresh rates (720hz)
            m_fields->smth_dt += (safe_dt - m_fields->smth_dt) * 0.5f;
            safe_dt = m_fields->smth_dt;
        }

        if (g_stutter_grace > 0) g_stutter_grace--;
        else g_is_lagging_bad = false;

        PlayLayer::update(safe_dt);
    }
};

class $modify(LevelEditorLayer) {
    struct Fields {
        float ed_smth_dt = target_720_dt;
        bool ed_init = true;
    };

    bool init(GJGameLevel * lvl, bool unk) {
        if (!LevelEditorLayer::init(lvl, unk)) return false;
        m_fields->ed_smth_dt = target_720_dt;
        m_fields->ed_init = true;
        return true;
    }

    void update(float dt) {
        // editor logic is slightly different to prevent 'drag stutter'
        float d = dt;
        if (d > PHYSICAL_DT_LIMIT) d = PHYSICAL_DT_LIMIT;

        if (m_fields->ed_init) {
            m_fields->ed_smth_dt = d;
            m_fields->ed_init = false;
        }
        else {
            // editor uses a slightly lazier smoothing for stability
            m_fields->ed_smth_dt += (d - m_fields->ed_smth_dt) * 0.35f;
            d = m_fields->ed_smth_dt;
        }
        LevelEditorLayer::update(d);
    }
};

class $modify(CCParticleSystem) {
    void update(float dt) {
        // absolute garbage memory management from rob top here.
        // we skip every other particle update if the game is stuttering.
        static int skip_counter = 0;

        if (g_is_lagging_bad && (++skip_counter % 2 == 0)) {
            return;
        }

        CCParticleSystem::update(dt);
    }
};
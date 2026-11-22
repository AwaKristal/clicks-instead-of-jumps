#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

using namespace geode::prelude;

static int g_clicks = 0;

class $modify(PlayLayer) {
    void resetLevel() {
        if (!m_isPracticeMode || m_checkpointArray->count() == 0) {
            g_clicks = 0;
        }
        PlayLayer::resetLevel();
    }
};

class $modify(GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);

        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            return;
        }

        if (down && PlayLayer::get()) {
            g_clicks++;
        }
    }
};

class $modify(EndLevelLayer) {
    void customSetup() {
        EndLevelLayer::customSetup();

        if (!Mod::get()->getSettingValue<bool>("enabled")) {
            return;
        }

        bool keepJumps = Mod::get()->getSettingValue<bool>("keep-jumps");
        std::string clicksText = fmt::format("Clicks: {}", g_clicks);

        CCLabelBMFont* jumpsLabel = typeinfo_cast<CCLabelBMFont*>(
            m_mainLayer->getChildByIDRecursive("jumps-label")
        );

        if (!jumpsLabel) {
            for (auto* node : CCArrayExt<CCNode*>(m_mainLayer->getChildren())) {
                if (auto* label = typeinfo_cast<CCLabelBMFont*>(node)) {
                    std::string text = label->getString();
                    if (text.starts_with("Jumps:")) {
                        jumpsLabel = label;
                        break;
                    }
                }
            }
        }

        if (!jumpsLabel) {
            return;
        }

        if (!keepJumps) {
            jumpsLabel->setString(clicksText.c_str());
            return;
        }

        auto clicksLabel = CCLabelBMFont::create(clicksText.c_str(), "goldFont.fnt");
        clicksLabel->setID("clicks-label"_spr);
        clicksLabel->setScale(jumpsLabel->getScale());

        CCPoint pos = jumpsLabel->getPosition();
        clicksLabel->setPosition({ pos.x, pos.y - 24.f });

        if (auto* parent = jumpsLabel->getParent()) {
            parent->addChild(clicksLabel);
        }

        if (auto* timeLabel = typeinfo_cast<CCLabelBMFont*>(
            m_mainLayer->getChildByIDRecursive("time-label")
        )) {
            timeLabel->setPositionY(timeLabel->getPositionY() - 22.f);
        }
    }
};
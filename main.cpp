#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/Notification.hpp>

#include <algorithm>
#include <filesystem>
#include <random>
#include <string>
#include <vector>

using namespace geode::prelude;

namespace betterclick {
	namespace fs = std::filesystem;

	static std::vector<std::string> g_clicks;
	static std::vector<std::string> g_releases;
	static std::mt19937 g_rng{std::random_device{}()};

	static std::string lower(std::string s) {
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
		return s;
	}

	static bool isAudio(fs::path const& p) {
		auto ext = lower(p.extension().string());
		return ext == ".wav" || ext == ".mp3" || ext == ".ogg";
	}

	// Folder clickpack: <config dir>/clickpack/
	// Semua file audio di dalamnya (termasuk subfolder) dipindai.
	// Jika path file/folder mengandung kata "release" -> suara release,
	// selain itu -> suara click.
	static fs::path packDir() {
		return Mod::get()->getConfigDir(true) / "clickpack";
	}

	static void reloadPack() {
		g_clicks.clear();
		g_releases.clear();

		auto root = packDir();
		std::error_code ec;
		fs::create_directories(root, ec);

		for (auto it = fs::recursive_directory_iterator(root, ec);
			 !ec && it != fs::recursive_directory_iterator(); it.increment(ec)) {
			if (ec) break;
			if (!it->is_regular_file(ec) || !isAudio(it->path())) continue;

			auto rel = lower(fs::relative(it->path(), root, ec).generic_string());
			auto full = it->path().string();
			if (rel.find("release") != std::string::npos) {
				g_releases.push_back(full);
			} else {
				g_clicks.push_back(full);
			}
		}

		log::info("Better Click: {} click, {} release dimuat dari {}", g_clicks.size(), g_releases.size(), root.string());
	}

	static std::string pick(std::vector<std::string> const& list) {
		if (list.empty()) return "";
		std::uniform_int_distribution<size_t> d(0, list.size() - 1);
		return list[d(g_rng)];
	}

	static void play(bool release) {
		auto mod = Mod::get();
		if (!mod->getSettingValue<bool>("enabled")) return;
		if (release && !mod->getSettingValue<bool>("play-release")) return;

		std::string path = mod->getSettingValue<std::string>(release ? "custom-release" : "custom-click");
		std::error_code ec;
		if (path.empty() || !fs::exists(path, ec)) {
			path.clear();
			if (mod->getSettingValue<bool>("use-clickpack")) {
				path = pick(release ? g_releases : g_clicks);
			}
		}
		if (path.empty()) return;

		float volume = static_cast<float>(mod->getSettingValue<double>(release ? "release-volume" : "click-volume"));
		float var = static_cast<float>(mod->getSettingValue<double>("pitch-variation"));
		float speed = 1.f;
		if (var > 0.f) {
			std::uniform_real_distribution<float> d(-var, var);
			speed += d(g_rng);
		}

		FMODAudioEngine::get()->playEffect(path, speed, 0.f, volume);
	}
}

$on_mod(Loaded) {
	betterclick::reloadPack();
}

// Deteksi klik / lepas saat bermain level
class $modify(BCBaseGameLayer, GJBaseGameLayer) {
	void handleButton(bool down, int button, bool isPlayer1) {
		GJBaseGameLayer::handleButton(down, button, isPlayer1);
		if (button != 1) return; // 1 = tombol lompat
		if (!typeinfo_cast<PlayLayer*>(this)) return;
		betterclick::play(!down);
	}
};

// Tombol tes di menu utama: Click, Release, Reload, Settings
class $modify(BCMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		auto menu = CCMenu::create();
		menu->setPosition({0.f, 0.f});

		auto winSize = CCDirector::get()->getWinSize();
		float x = 45.f;
		float y = winSize.height - 30.f;
		float step = 28.f;

		auto addBtn = [&](char const* text, SEL_MenuHandler cb) {
			auto spr = ButtonSprite::create(text);
			spr->setScale(0.5f);
			auto btn = CCMenuItemSpriteExtra::create(spr, this, cb);
			btn->setPosition({x, y});
			menu->addChild(btn);
			y -= step;
		};

		addBtn("Click", menu_selector(BCMenuLayer::onBCClick));
		addBtn("Release", menu_selector(BCMenuLayer::onBCRelease));
		addBtn("Reload", menu_selector(BCMenuLayer::onBCReload));
		addBtn("Setting", menu_selector(BCMenuLayer::onBCSettings));

		this->addChild(menu, 100);
		return true;
	}

	void onBCClick(CCObject*) {
		betterclick::play(false);
	}

	void onBCRelease(CCObject*) {
		betterclick::play(true);
	}

	void onBCReload(CCObject*) {
		betterclick::reloadPack();
		auto msg = fmt::format("{} click, {} release dimuat", betterclick::g_clicks.size(), betterclick::g_releases.size());
		Notification::create(msg, NotificationIcon::Success)->show();
	}

	void onBCSettings(CCObject*) {
		openSettingsPopup(Mod::get());
	}
};

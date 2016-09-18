#include "editor_root_stage.h"
#include "console/console_window.h"
#include "halley_editor.h"
#include "ui/taskbar/taskbar.h"
#include "halley/tools/assets/check_assets_task.h"

using namespace Halley;

EditorRootStage::EditorRootStage(HalleyEditor& editor)
	: editor(editor)
	, tasks(std::make_unique<EditorTaskSet>())
{
}

EditorRootStage::~EditorRootStage()
{
}

void EditorRootStage::init()
{
	tasks->addTask(EditorTaskAnchor(std::make_unique<CheckAssetsTask>(editor.getProject(), editor.isHeadless())));

	if (!editor.isHeadless()) {
		initSprites();
		taskBar = std::make_unique<TaskBar>(getResources());
		console = std::make_unique<ConsoleWindow>(getResources());
	}

	/*
	auto audio = getAPI().audioOutput;
	audio->openAudioDevice(AudioSpec(48000, 2, 4096, AudioSampleFormat::Float), [](gsl::span<gsl::byte> span) {
		gsl::span<float> dst(reinterpret_cast<float*>(span.data()), span.size_bytes() / sizeof(float));
		size_t nSamples = size_t(dst.size() / 2);
		float harmonic = 32.0f;
		float scale = harmonic * 6.283185307179586476925286766559f / nSamples;
		for (size_t i = 0; i < nSamples; i++) {
			float phase = i * scale;
			float v = ::sin(phase) * 0.3f;
			dst[i * 2] = v;
			dst[i * 2 + 1] = v;
		}
	});
	audio->startPlayback();
	*/
}

void EditorRootStage::onVariableUpdate(Time time)
{
	tasks->update(time);

	if (editor.isHeadless() && tasks->getTasks().empty()) {
		getCoreAPI().quit();
	}

	if (taskBar) {
		taskBar->update(tasks->getTasks(), time);
	}

	if (console) {
		console->update(*getInputAPI().getKeyboard());
	}
}

void EditorRootStage::onRender(RenderContext& context) const
{
	context.bind([&](Painter& painter)
	{
		auto view = Rect4f(painter.getViewPort());

		// Background
		Sprite bg = background;
		bg.setTexRect(view).setSize(view.getSize()).draw(painter);
		halleyLogo.clone().setPos(Vector2f(getVideoAPI().getWindow().getDefinition().getSize() / 2)).draw(painter);

		// Taskbar
		taskBar->draw(painter);

		if (console) {
			console->draw(painter, Rect4f(view.getTopLeft() + Vector2f(16, 16), view.getBottomRight() - Vector2f(16, 80)));
		}
	});
}

void EditorRootStage::initSprites()
{
	{
		// Background
		{
			auto matRaw = std::make_shared<Material>(getResource<MaterialDefinition>("scanlines"));
			auto& mat = *matRaw;
			mat["u_col0"] = Colour4f(0.08f);
			mat["u_col1"] = Colour4f(0.07f);
			mat["u_distance"] = 6.0f;
			background = Sprite().setMaterial(matRaw).setPos(Vector2f(0, 0));
		}
	}

	{
		// Halley logo
		auto col = Colour4f(0.065f);
		//auto col = Colour4f(0.9882f, 0.15686f, 0.27843f, 1);
		halleyLogo = Sprite()
			.setImage(getResources(), "halley_icon_dist.png", "distance_field_sprite")
			.setPivot(Vector2f(0.5f, 0.5f))
			.setColour(col)
			.setScale(Vector2f(8, 8))
			.setPos(Vector2f(640, 360));
		auto& mat = halleyLogo.getMaterial();
		mat["u_smoothness"] = 0.1f;
		mat["u_outline"] = 0.0f;
		mat["u_outlineColour"] = col;
	}
}
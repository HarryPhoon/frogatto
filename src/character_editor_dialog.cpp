#include <boost/bind.hpp>

#include "border_widget.hpp"
#include "button.hpp"
#include "character_editor_dialog.hpp"
#include "editor.hpp"
#include "foreach.hpp"
#include "frame.hpp"
#include "grid_widget.hpp"
#include "image_widget.hpp"
#include "label.hpp"
#include "raster.hpp"

namespace editor_dialogs
{

character_editor_dialog::character_editor_dialog(editor& e)
  : gui::dialog(graphics::screen_width() - 160, 160, 160, 440), editor_(e), first_index_(-1)
{
	if(editor_.all_characters().empty() == false) {
		category_ = editor_.all_characters().front().category;
	}

	init();
}

void character_editor_dialog::init()
{
	clear();
	using namespace gui;
	set_padding(20);

	const frame& frame = *editor_.all_characters()[editor_.get_object()].preview_frame;
//	image_widget* preview = new image_widget(frame.img());
//	preview->set_dim(150, 150);
//	preview->set_area(frame.area());
//	add_widget(widget_ptr(preview), 10, 10);

	button* category_button = new button(widget_ptr(new label(category_, graphics::color_white())), boost::bind(&character_editor_dialog::show_category_menu, this));
	add_widget(widget_ptr(category_button), 10, 10);

	grid_ptr grid(new gui::grid(3));
	int index = 0;
	first_index_ = -1;
	foreach(const editor::enemy_type& c, editor_.all_characters()) {
		if(c.category == category_) {
			if(first_index_ == -1) {
				first_index_ = index;
			}

			image_widget* preview = new image_widget(c.preview_frame->img());
			preview->set_dim(40, 40);
			preview->set_area(c.preview_frame->area());
			button_ptr char_button(new button(widget_ptr(preview), boost::bind(&character_editor_dialog::set_character, this, index)));
			char_button->set_dim(44, 44);
			grid->add_col(gui::widget_ptr(new gui::border_widget(char_button, index == editor_.get_object() ? graphics::color(255,255,255,255) : graphics::color(0,0,0,0))));
		}

		++index;
	}

	grid->finish_row();
	add_widget(grid);

}

void character_editor_dialog::show_category_menu()
{
	using namespace gui;
	gui::grid* grid = new gui::grid(2);
	grid->set_show_background(true);
	grid->set_hpad(10);
	grid->allow_selection();
	grid->register_selection_callback(boost::bind(&character_editor_dialog::close_context_menu, this, _1));

	std::set<std::string> categories;
	foreach(const editor::enemy_type& c, editor_.all_characters()) {
		if(categories.count(c.category)) {
			continue;
		}

		categories.insert(c.category);

		image_widget* preview = new image_widget(c.preview_frame->img());
		preview->set_dim(28, 28);
		preview->set_area(c.preview_frame->area());
		grid->add_col(widget_ptr(preview))
		     .add_col(widget_ptr(new label(c.category, graphics::color_white())));
		grid->register_row_selection_callback(boost::bind(&character_editor_dialog::select_category, this, c.category));
	}

	int mousex, mousey;
	SDL_GetMouseState(&mousex, &mousey);
	if(mousex + grid->width() > graphics::screen_width()) {
		mousex = graphics::screen_width() - grid->width();
	}

	if(mousey + grid->height() > graphics::screen_height()) {
		mousey = graphics::screen_height() - grid->height();
	}

	mousex -= x();
	mousey -= y();

	remove_widget(context_menu_);
	context_menu_.reset(grid);
	add_widget(context_menu_, mousex, mousey);
}

void character_editor_dialog::set_character(int index)
{
	category_ = editor_.all_characters()[index].category;
	editor_.set_object(index);
	init();
}

void character_editor_dialog::close_context_menu(int index)
{
	remove_widget(context_menu_);
	context_menu_.reset();
}

void character_editor_dialog::select_category(const std::string& category)
{
	category_ = category;
	init();
	if(first_index_ >= 0) {
		set_character(first_index_);
	}
}

}

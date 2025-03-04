//
// A simple editor for putting HM stuff around prims.
//

class HmTab : public ModeTab
{
	private:

		void draw_prim(std::uint16_t prim);
		void draw_grid(std::uint16_t prim);
		void draw_cog (std::uint16_t prim);

	public:

		//
		// The bare minimum...
		//

		HmTab(EditorModule *parent);
	   ~HmTab();

	    void	DrawTabContent          ();
		void	HandleTab               (MFPoint *current_point);
		std::uint16_t	HandleTabClick          (std::uint8_t flags, MFPoint *clicked_point);
		void	HandleControl           (std::uint16_t control_id);
		void	DrawModuleContent       (std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h);
		std::int32_t	HandleModuleContentClick(MFPoint *clicked_point,std::uint8_t flags,std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h);
		std::uint8_t   RedrawModuleContent;

		//
		// Extra stuff...
		//
};

// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
#pragma warning(push)
#pragma warning(disable:4244)
  zCView* effect   = Null;
  zCView* postview = Null;

  HOOK Ivk_Render_Union PATCH( &oCGame::Render, &oCGame::Render_Union );

  static float timeOnScreen = 0.0f;
  static float position     = 8192;
  static float alpha        = 0.0f;
  static bool  animations   = false;
  static int   miniX;
  static int   miniY;

  void oCGame::Render_Union() {
    if( !ogame->GetGameWorld() )
      return THISCALL( Ivk_Render_Union )();

    bool_t toggled_f10 = zKeyToggled( s_HotKey );
    if( toggled_f10 ) {
      screen->RemoveItem( effect );
      screen->RemoveItem( postview );
    }



    THISCALL( Ivk_Render_Union )();

    if( toggled_f10 ) {
      CreateScreenShot();
      screen->InsertItem( effect );
      alpha = 200.0f;
      effect->alpha;
    }

    static zCTexture* tex;
    static string name;
    ScreenShotPop( tex, name );
    if( tex ) {
      miniX = screen->nax( 8192 ) / 5;
      miniY = screen->nay( 8192 ) / 5;
      animations = true;
      postview->SetSize( zPixelX( miniX ), zPixelY( miniY ) );
      postview->InsertBack( tex );
      timeOnScreen = 4000.0f;
      tex->Release();
      postview->ClrPrintwin();
      postview->Print( 0, 0, name );
    }

    screen->InsertItem( postview );

    if( animations ) {
      if( timeOnScreen > 0.0f ) {
        timeOnScreen -= ztimer->frameTimeFloat;
        int px = 8192 - zPixelX( miniX );
        position -= miniX / 100.0f * ztimer->frameTimeFloat;
        if( position < px )
          position = px;

        postview->SetPos( position, 1000 );
      }
      else {
        int px = 8200;
        position += miniX / 100.0f * ztimer->frameTimeFloat;
        if( position > 8192 ) {
          position = px;
          animations = false;
        }

        postview->SetPos( position, 1000 );
      }
    }

    if( alpha > 0.0f ) {
      alpha -= ztimer->frameTimeFloat;
      effect->alpha = max( 0, (int)alpha );

      if( effect->alpha == 0 )
        screen->RemoveItem( effect );
    }
  }
#pragma warning(default:4244)
#pragma warning(push)
}
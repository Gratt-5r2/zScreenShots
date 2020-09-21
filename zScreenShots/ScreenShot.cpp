// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  CMutex screenShotsQueue;

  void CreateTextureFromScreenShot( zCTextureConvert* texConv, string file ) {
    if( !texConv ) {
      cmd << "no tex conv" << endl;
      return;
    }

    zCTextureInfo texInfo = texConv->GetTextureInfo();
    texInfo.texFormat = zCTexture::CalcNextBestTexFormat( zRND_TEX_FORMAT_RGB_565 );
    texInfo.numMipMap = 1;
    texInfo.sizeX = 512;
    texInfo.sizeY = 256;
    ((zCTexConGeneric*)texConv)->ConvertTextureFormat( texInfo );

    zCTexture* tex = new zCTex_D3D();
    zCTextureExchange::CopyContents( texConv, tex );
    ScreenShotPush( tex, file );
  }

  string CheckFileName( string name ) {
    if( _access( "ScreenShots/" + name + ".jpg", 0 ) != 0 )
      return name;

    for( uint i = 2; true; i++ ) {
      string nextName = name + "_v" + i;
      if( _access( "ScreenShots/" + nextName + ".jpg", 0 ) != 0 )
        return nextName;
    }
  }



  CLSID myClsId;
  void CreateScreenShotAsync( zCTextureConvert* texConv ) {
    screenShotsQueue.Enter();
    time_t baseTime = time( Null );
    tm* now = localtime( &baseTime );

    string timeFormat = string::Combine(
      "%u_%u_%u_%u_%u_%u",
      now->tm_year + 1900,
      now->tm_mon + 1,
      now->tm_mday,
      now->tm_hour,
      now->tm_min,
      now->tm_sec
    );

    string fileName = "ScreenShot_" + timeFormat;
    fileName = CheckFileName( fileName ) + ".jpg";

    //cmd << "begin" << endl;
    if( _access( "ScreenShots", 0 ) != 0 )
      CreateDirectory( "ScreenShots", 0 );

    zCTextureInfo info = texConv->GetTextureInfo();
    info.texFormat = zRND_TEX_FORMAT_BGRA_8888;
    ((zCTexConGeneric*)texConv)->ConvertTextureFormat( info );
    zCTextureConvert* pNewConv = zrenderer->CreateTextureConvert();
    zCTextureExchange::CopyContents( texConv, pNewConv );
    ((zCTexConGeneric*)texConv)->SetDetailTextureMode( 0 );
    ((zCTexConGeneric*)texConv)->ConvertTextureFormat( info );
    int nPitch = info.sizeX * 4;
    int nImageSize = nPitch * info.sizeY;
    void* pb = (void*) ::GlobalAlloc( GMEM_FIXED, nImageSize );
    pNewConv->CopyTextureDataTo( 0, pb, info.sizeX * 4 );
    delete pNewConv;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup( &gdiplusToken, &gdiplusStartupInput, NULL );
    HBITMAP hBitmap = CreateBitmap( info.sizeX, info.sizeY, 1, 32, pb );
    ::GlobalFree( pb );
    Bitmap* image = new Bitmap( hBitmap, NULL );
    wstring outName = L"ScreenShots/" + fileName.AToW();

    static bool once = true;
    if( once ) {
      once = false;
      int retVal = GetEncoderClsID( L"image/jpeg", &myClsId );
    }

    Gdiplus::Status s = image->Save( outName, &myClsId );
    delete image;
    GdiplusShutdown( gdiplusToken );

    if( s == Gdiplus::Status::Ok ) {
      //cmd << "ScreenShot saved" << endl;
      //ScreenShotPush( "ScreenShot saved! " + fileName );
    }
    else {
      cmd << "ScreenShot NOT saved" << endl;
      fileName = "NOT SAVED :(";
      //ScreenShotPush( "ScreenShot NOT saved! " + fileName );
    }

    CreateTextureFromScreenShot( texConv, fileName );
    screenShotsQueue.Leave();
  }

  void CreateScreenShot() {
    zCTextureConvert* pTexConv = zrenderer->CreateTextureConvert();
    zrenderer->Vid_GetFrontBufferCopy( *pTexConv );

    //CreateTextureFromScreenShot( pTexConv );
    CThread thread( CreateScreenShotAsync );
    thread.Detach( pTexConv );
  };




}
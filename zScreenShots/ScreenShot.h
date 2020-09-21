// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
  static int s_HotKey = KEY_F10;



  enum ScreenShotFormat {
    JPEG,
    PNG,
    BMP
  };



  CMutex screenShotArrayMutex;

  struct TSavedInfo {
    zCTexture* tex;
    string file;
  };

  Array<TSavedInfo> screenShotSaved;



  void ScreenShotPush( zCTexture* tex, string file ) {
    screenShotArrayMutex.Enter();
    TSavedInfo& info = screenShotSaved.Create();
    info.tex = tex;
    info.file = file;
    screenShotArrayMutex.Leave();
  }



  void ScreenShotPop( zCTexture*& tex, string& file ) {
    screenShotArrayMutex.Enter();
    if( !screenShotSaved.GetNum() ) {
      tex = Null;
      screenShotArrayMutex.Leave();
      return;
    }

    TSavedInfo& info = screenShotSaved.GetFirst();
    tex = info.tex;
    file = info.file;
    screenShotSaved.RemoveAt( 0 );
    screenShotArrayMutex.Leave();
  }



  int GetEncoderClsID( const WCHAR* format, CLSID* pClsid ) {
    UINT num  = 0; // number of image encoders
    UINT size = 0; // size of the image encoder array in bytes

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize( &num, &size );
    if( size == 0 )
      return -1;  // Failure

    pImageCodecInfo = (ImageCodecInfo*)(malloc( size ));
    if( pImageCodecInfo == NULL )
      return -1;  // Failure

    GetImageEncoders( num, size, pImageCodecInfo );

    for( UINT j = 0; j < num; ++j ) {
      if( wcscmp( pImageCodecInfo[j].MimeType, format ) == 0 ) {
        *pClsid = pImageCodecInfo[j].Clsid;
        free( pImageCodecInfo );
        return j;  // Success
      }
    }
    free( pImageCodecInfo );
    return -1;  // Failure
  }
}
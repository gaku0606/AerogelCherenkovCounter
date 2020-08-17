//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Geometry.cc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "Geometry.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpticalPhysics.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
//------------------------------------------------------------------------------
  Geometry::Geometry() {}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
  Geometry::~Geometry() {}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
  G4VPhysicalVolume* Geometry::Construct()
//------------------------------------------------------------------------------
{
  return ConstructDetector();
}

//------------------------------------------------------------------------------
  G4VPhysicalVolume* Geometry::ConstructDetector()
//------------------------------------------------------------------------------
{
  G4double Z, A, density;

// Get pointer to 'Material Manager'
   G4NistManager* materi_Man = G4NistManager::Instance();

  //************************
  //  マテリアルを定義
  //************************
	//----------
	// Element
	//----------
	G4Element* fH = new G4Element("H", "H", Z = 1., A = 1.01*g / mole);
	G4Element* fC = new G4Element("C", "C", Z = 6., A = 12.01*g / mole);
	G4Element* fN = new G4Element("N", "N", Z = 7., A = 14.01*g / mole);
	G4Element* fO = new G4Element("O", "O", Z = 8., A = 16.00*g / mole);

	//アルミニウム
	G4Material* fAl = new G4Material("Al", Z = 13., A = 26.98*g / mole, density = 2.7*g / cm3);
	//真空
	G4Material* fVacuum = new G4Material("Vacuum", Z = 1., A = 1.01*g / mole, density = 1.e-25*g / cm3,
		kStateGas, 0.1*kelvin, 1.e-19*pascal);

	//大気 こういう書き方で混合ガスが定義できることを覚えておく
	G4Material* fAir = new G4Material("Air", density = 1.29*g / cm3, 2);
	fAir->AddElement(fN, 70 * perCent);
	fAir->AddElement(fO, 30 * perCent);
	//ポリスチレン
	G4Material* fPstyrene = new G4Material("Polystyrene", density = 1.03*g / cm3, 2);
	fPstyrene->AddElement(fC, 8);
	fPstyrene->AddElement(fH, 8);
	//-------
	// 真空と大気
	//-------
	G4double vacuum_Energy[] = { 2.0*eV,7.0*eV,7.14*eV };
	const G4int vacnum = sizeof(vacuum_Energy) / sizeof(G4double);
	G4double vacuum_RIND[] = { 1.,1.,1. };
	assert(sizeof(vacuum_RIND) == sizeof(vacuum_Energy));
	G4MaterialPropertiesTable *vacuum_mt = new G4MaterialPropertiesTable();
	vacuum_mt->AddProperty("RINDEX", vacuum_Energy, vacuum_RIND, vacnum);
	fVacuum->SetMaterialPropertiesTable(vacuum_mt);
	fAir->SetMaterialPropertiesTable(vacuum_mt);//Give air the same rindex


  //-------
	// WLS
	//-------
	G4double wls_Energy[] = { 2.00*eV,2.87*eV,2.90*eV,3.47*eV };
	const G4int wlsnum = sizeof(wls_Energy) / sizeof(G4double);
//-------
	// ポリスチレン
	//-------
	G4double rIndexPstyrene[] = { 1.5, 1.5, 1.5, 1.5 };
	assert(sizeof(rIndexPstyrene) == sizeof(wls_Energy));
	G4double absorption1[] = { 100.*cm, 100.*cm, 100.*cm, 100.*cm };
	assert(sizeof(absorption1) == sizeof(wls_Energy));
	G4double scintilFast[] = { 0.00, 0.00, 1.00, 1.00 };
	assert(sizeof(scintilFast) == sizeof(wls_Energy));
	G4MaterialPropertiesTable* fMPTPStyrene = new G4MaterialPropertiesTable();
	fMPTPStyrene->AddProperty("RINDEX", wls_Energy, rIndexPstyrene, wlsnum);
	fMPTPStyrene->AddProperty("ABSLENGTH", wls_Energy, absorption1, wlsnum);
	//fMPTPStyrene->AddProperty("FASTCOMPONENT", wls_Energy, scintilFast, wlsnum);
	fMPTPStyrene->AddConstProperty("SCINTILLATIONYIELD", 0.0/ MeV);//1000. / MeV);//10. / keV);
	//fMPTPStyrene->AddConstProperty("RESOLUTIONSCALE", 1.0);
	//fMPTPStyrene->AddConstProperty("FASTTIMECONSTANT", 10.*ns);
	fPstyrene->SetMaterialPropertiesTable(fMPTPStyrene);
	// Set the Birks Constant for the Polystyrene scintillator
	//fPstyrene->GetIonisation()->SetBirksConstant(0.126*mm / MeV);
  

  // Define logical volume of aerogel
  G4Element* elC  = new G4Element("Carbon",  "C",  Z=6.,  A= 12.011 *g/mole);
  G4Element* elSi = new G4Element("Silicon", "Si", Z=14., A= 28.0855*g/mole);
  G4Element* elO  = new G4Element("Oxygen",  "O",  Z=8.,  A= 15.9994*g/mole);

  // quartz (SiO2)
  density = 2.64 * g/cm3;
  G4Material* SiO2 = new G4Material("Quartz",density, 2);
  SiO2->AddElement(elSi, 1);
  SiO2->AddElement(elO, 2);
  //H2O
  G4Material* H2O = materi_Man->FindOrBuildMaterial("G4_WATER");
 
  density = 0.200*g/cm3;
  G4Material* materi_Aerogel = 
    new G4Material("Aerogel", density, 3);//name, density, number of componets
  materi_Aerogel->AddMaterial(SiO2,  62.5*perCent);//fractionmass
  materi_Aerogel->AddMaterial(H2O, 37.4*perCent  );
  materi_Aerogel->AddElement(elC, 0.1*perCent);

  const G4int NUMENTRIES = 2;
  G4double PPSINT1[NUMENTRIES ]={1.96*eV, 3.06*eV};//633 nm, 405 nm
  G4double RINDEX2[NUMENTRIES]={1.05, 1.05};//Aerogel index
  G4double ABSORPTION[NUMENTRIES]={40.0*mm, 40.0*mm};//光吸収係数だが、とりあえず透過長
  //G4double SCINTILLATION[NUMENTRIES]={0.5*MeV};//???
  G4MaterialPropertiesTable *myMPT2
    = new G4MaterialPropertiesTable();
  
  myMPT2->AddProperty("RINDEX", PPSINT1, RINDEX2, NUMENTRIES);
  myMPT2->AddProperty("ABSLENGTH",PPSINT1,ABSORPTION,NUMENTRIES);
  myMPT2->AddConstProperty("SCINTILLATIONYIELD",0./MeV);
  //myMPT2->AddConstProperty("RESOULUTIONSCALE", 1.0);
  materi_Aerogel->SetMaterialPropertiesTable(myMPT2);
  //fPstyrene->SetMaterialPropertiesTable(myMPT2);

  //**********************
  // LogVolを定義
  //***********************
// Define 'World Volume'
   // Define the shape of solid
   G4double leng_X_World = 2.0*m;         // X-full-length of world 
   G4double leng_Y_World = 2.0*m;         // Y-full-length of world 
   G4double leng_Z_World = 2.0*m;         // Z-full-length of world 
   G4Box* solid_World = 
     new G4Box("Solid_World", leng_X_World/2.0, leng_Y_World/2.0, leng_Z_World/2.0);

   // Define logical volume
   G4Material* materi_World = materi_Man->FindOrBuildMaterial("G4_AIR");
   G4LogicalVolume* logVol_World = 
     new G4LogicalVolume(solid_World, fAir, "LogVol_World");
   logVol_World->SetVisAttributes (G4VisAttributes::Invisible);

   // Placement of logical volume
   G4int copyNum_World = 0;               // Set ID number of world
   G4PVPlacement* physVol_World  = 
     new G4PVPlacement(G4Transform3D(), "PhysVol_World", logVol_World, 0, 
                       false, copyNum_World);

  // アルミケース
  const G4double AlThickness = 0.1 * cm;//厚み
  G4double leng_x_Al = 15.0 * cm + AlThickness;
  G4double leng_y_Al = 15.0 * cm + AlThickness;
  G4double leng_z_Al = 2.0 * cm + AlThickness;
  G4Box* solid_AlCase = new G4Box("Solid_Al", leng_x_Al/2., leng_y_Al/2., leng_z_Al/2.);
  G4LogicalVolume* LogVol_AlCase = new G4LogicalVolume(solid_AlCase, fAl, "LogVol_AlCase", 0, 0, 0);
  	//論理ボリュームの色を設定、r,b,g,a
	LogVol_AlCase->SetVisAttributes(new G4VisAttributes(TRUE, G4Color(1.0, 1.0, 1.0, 1.0)));
  G4ThreeVector vec3_AlCase = G4ThreeVector(0.0 * cm, 0.0* cm, 0.0 * cm);
  G4RotationMatrix rotMat_AlCase = G4RotationMatrix();
  G4Transform3D trans3D_ALCase = G4Transform3D(rotMat_AlCase, vec3_AlCase);  
  // Placement of logical volume of Al case
  G4int copyNum_Alcase = 900;
  G4PVPlacement* physVol_AlCase = 
    new G4PVPlacement(trans3D_ALCase, "PhtsVol_AlCase", LogVol_AlCase, physVol_World, false, copyNum_Alcase);
    
  //アルミケース内の空気
  G4double leng_x_Air = 15.0 * cm;
  G4double leng_y_Air = 15.0 * cm;
  G4double leng_z_Air = 2.0 * cm;
  G4Box* solid_Air = new G4Box("Solid_Air", leng_x_Air/2., leng_y_Air/2., leng_z_Air/2.);
  G4LogicalVolume* LogVol_Air = new G4LogicalVolume(solid_Air, fAir, "LogVol_Air", 0, 0, 0);
  //論理ボリュームの色を設定、r,b,g,a
	LogVol_Air->SetVisAttributes(new G4VisAttributes(TRUE, G4Color(1.0, 1.0, 1.0, 1.0)));

  //Placement of logical volume of Air
  G4int copyNum_Air = 950;
  G4PVPlacement* physVol_Air = 
    new G4PVPlacement(trans3D_ALCase, "PhysVol_Air", LogVol_Air, physVol_AlCase, false, copyNum_Air);

  //チェレンコフ光を出す物質定義
  //define shape of detector
  G4double leng_x_aerogel = 15. * cm;
  G4double leng_y_aerogel = 15. * cm;
  G4double leng_z_aerogel = 1.0 * cm;
  G4Box* solid_aeogel
   = new G4Box("Solid_Aerogel", leng_x_aerogel/2.0, leng_y_aerogel/2.0, leng_z_aerogel / 2.0);

  G4LogicalVolume* logVol_Aerogel = 
    new G4LogicalVolume(solid_aeogel, materi_Aerogel,//fPstyrene,
     "LogVol_Aerogel", 0, 0, 0);

  //論理ボリュームの色を設定、r,b,g,a
	logVol_Aerogel->SetVisAttributes(new G4VisAttributes(TRUE, G4Color(0.8, 0.8, 1.0, 1.0)));

  // Placement of logical volume of Aerogel
  G4double pos_x_aerogel_LogV = 0.0*cm;
  G4double pos_y_aerogel_LogV = 0.0*cm;
  G4double pos_z_aerogel_LogV = -0.5*cm;
  G4ThreeVector vec3_logV_aerogel = G4ThreeVector(pos_x_aerogel_LogV, pos_y_aerogel_LogV, pos_z_aerogel_LogV);
  G4RotationMatrix rotMat_LogV_aerogel = G4RotationMatrix();
  G4Transform3D trans3D_LogV_aerogel = G4Transform3D(rotMat_LogV_aerogel, vec3_logV_aerogel);


  G4int copyNum_LogV_aerogel = 1001;  //set ID number of LogV
  G4PVPlacement *physVol_Aerogel = new G4PVPlacement(trans3D_LogV_aerogel, "PhysVol_Aerogel", logVol_Aerogel, physVol_Air, 
          false, copyNum_LogV_aerogel);


	//-------------------------
	// 反射材の設定
	//-------------------------
  G4OpticalSurface* AlSurface = new G4OpticalSurface("AlSurface");
  AlSurface->SetType(dielectric_metal);
  AlSurface->SetFinish(polished);
  AlSurface->SetModel(glisur);
  const G4int AlNum = 2;
  G4double PP_Al[AlNum] = { 2.0*eV, 3.5*eV};
  G4double reflecivity_Al[AlNum] = { 1.0, 1.0};
  G4double efficiency_Al[AlNum] = { 0.0, 0.0};
  G4MaterialPropertiesTable* AlSurfaceProperty = new G4MaterialPropertiesTable();
  AlSurfaceProperty->AddProperty("REFLECTIVITY",PP_Al, reflecivity_Al, AlNum);
  AlSurfaceProperty->AddProperty("EFFICIENCY", PP_Al, efficiency_Al, AlNum);
  AlSurfaceProperty->AddProperty("RINDEX", vacuum_Energy, vacuum_RIND, vacnum);
  AlSurface->SetMaterialPropertiesTable(AlSurfaceProperty);
  new G4LogicalBorderSurface("AlSurface", physVol_Air, physVol_AlCase, AlSurface);

	G4OpticalSurface* wrapper = new G4OpticalSurface("wrapper");
	wrapper->SetType(dielectric_dielectric);//dielectric_metal);
	wrapper->SetFinish(ground);//polishedbackpainted);
	wrapper->SetModel(unified);//glisur);

	const G4int NUM = 2;
	G4double pp[NUM] = { 2.0 * eV, 3.5 *eV };
	G4double reflectivity[NUM] = { 1.0, 1.0 };
	G4double efficiency[NUM] = { 0.0, 0.0 };
	G4MaterialPropertiesTable* wrapperProperty = new G4MaterialPropertiesTable();
	wrapperProperty->AddProperty("REFLECTIVITY", pp, reflectivity, NUM);
	wrapperProperty->AddProperty("EFFICIENCY", pp, efficiency, NUM);
	wrapperProperty->AddProperty("RINDEX", vacuum_Energy, vacuum_RIND, vacnum);
	wrapper->SetMaterialPropertiesTable(wrapperProperty);

	//new G4LogicalSkinSurface("wrapper", logVol_ScintiBox, wrapper);
	new G4LogicalBorderSurface("wrapper", physVol_Aerogel, physVol_World, wrapper);

  // //OpAerogelSurfaceという名前のSurface状態を用意
  // G4OpticalSurface *OpAerogelSurface = new G4OpticalSurface("AerogelSurface");
  // //境界面情報を渡す
  // OpAerogelSurface->SetType(dielectric_dielectric);//不導体と不導体
  // OpAerogelSurface->SetFinish(ground);//ざらざらしている
  // OpAerogelSurface->SetModel(unified);

  // //エネルギー別の屈折率などを設定できる
  // const G4int NUM = 2;
  // G4double PP[NUM] = {1.96*eV, 3.06*eV};//405 nm
  // G4double RINDEX[NUM] = {1.0, 1.0};//屈折率
  // G4double reflectivity [NUM] = {1.0, 1.0};
  // G4double efficiency[NUM] = {0.0, 0.0};
  // G4double SPECULARLOBECONSTANT[NUM]={0.3, 0.3};//lobe
  // G4double SPECULARSPIKECONSTANT[NUM]={0.2};
  // G4double BACKSCATTERCONSTANT[NUM] = {0.2};//Backscattering
  // G4MaterialPropertiesTable* myST1 = new G4MaterialPropertiesTable();//各々の情報を置くtable
  // myST1->AddProperty("RINDEX", PP, RINDEX, NUM);
  // myST1->AddProperty("REFLECTIVITY", PP, reflectivity, NUM);
  // myST1->AddProperty("EFFICIENCY",PP, efficiency,NUM);
  // myST1->AddProperty("SPECULARLOBECONSTANT",PP,SPECULARLOBECONSTANT,NUM);
  // myST1->AddProperty("SPECULARSPIKECONSTANT",PP,SPECULARSPIKECONSTANT,NUM);
  // myST1->AddProperty("BACKSCATTERCONSTANT",PP,BACKSCATTERCONSTANT,NUM);
  // //反射材を使う時はREFLECTIONも追加
  // OpAerogelSurface->SetMaterialPropertiesTable(myST1);

  // //AeroelのsurfaceをSkin型で定義
  // G4LogicalSkinSurface* AerogelSurface 
  //   = new G4LogicalSkinSurface("AerogelSurface", logVol_Aerogel, OpAerogelSurface);
  
 
 //new G4LogicalBorderSurface("AerogelSurface", physVol_Aerogel, physVol_World, OpAeroelSurface);


//Return the physical world
   return physVol_World;
}

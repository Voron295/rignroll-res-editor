#include <Windows.h>
#include <iostream>
#include <string>


// Material flags
#define MATERIAL_HAS_TEXTURE 1
#define MATERIAL_HAS_TTX 2
#define MATERIAL_MOVE 4
#define MATERIAL_HAS_ROTATION 8
#define MATERIAL_HAS_MASK 0x10
#define MATERIAL_IS_TRANSPARENT 0x20
#define MATERIAL_HAS_ITX 0x40
#define MATERIAL_NOZ 0x80
#define MATERIAL_NOF 0x100
#define MATERIAL_NOTILE 0x200
#define MATERIAL_ATT 0x400
#define MATERIAL_ENV 0x800
#define MATERIAL_USE_COL 0x1000
#define MATERIAL_HAS_REFLECTION 0x2000
#define MATERIAL_HAS_ALPHAMIR 0x4000
#define MATERIAL_NOTILEU 0x8000
#define MATERIAL_NOTILEV 0x10000
#define MATERIAL_HAS_ROTATION_POINT 0x20000
#define MATERIAL_POWER 0x40000
#define MATERIAL_HAS_BUMP 0x80000
#define MATERIAL_HAS_WAVE 0x100000
#define MATERIAL_SPECULAR 0x200000

class Material
{
public:
	std::string m_sName;
	std::string m_sFullName;
	int m_dwFlags;
	int m_dwPower;
	int m_dwColorId;
	int m_dwInternalId;
	int m_dwInternalTexCoordsId;
	int m_dwParent;
	int m_dwTextureId;
	int m_dwMaskId;
	int m_dwEnvId;
	int someColor;
	float m_fMoveX;
	float m_fMoveY;
	float m_fTransparency;
	float m_fSpecular;
	float m_fEnv1;
	float m_fEnv2;
	float m_fReflection;
	float m_fRotation;
	float m_fRotationPointX;
	float m_fRotationPointY;

	class WinTreeItem *m_pTreeItem;
	class GameModule *m_pGameModule;

	void Load(FILE *f);
	bool Save(FILE *f);
	void Reset();

	Material();
	Material(Material &mat);
	Material &operator=(Material &mat);
	~Material();

	void SetTreeItem(class WinTreeItem *item);
	class WinTreeItem *GetTreeItem();
	void SetGameModule(class GameModule *item);
	class GameModule *GetGameModule();
};
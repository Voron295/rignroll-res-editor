#include "Material.h"
#include "FileManager.h"
#include "Mask.h"
#include "Texture.h"

Material::Material()
{
	m_dwFlags = 0;
	m_dwPower = 0;
	m_dwColorId = 0;
	m_dwInternalId = -1;
	m_dwInternalTexCoordsId = 0;
	m_dwParent = -1;
	m_dwTextureId = -1;
	m_dwMaskId = 0;
	m_dwEnvId = 0;
	someColor = 0;
	m_fMoveX = 0.f;
	m_fMoveY = 0.f;
	m_fTransparency = 0.f;
	m_fSpecular = 0.f;
	m_fEnv1 = 0.f;
	m_fEnv2 = 0.f;
	m_fReflection = 0.f;
	m_fRotation = 0.f;
	m_fRotationPointX = 0.f;
	m_fRotationPointY = 0.f;
	m_sName = "New Material";
}

Material::Material(Material &mat)
{
	m_sFullName = mat.m_sFullName;
	m_sName = mat.m_sName;
	m_dwFlags = mat.m_dwFlags;
	m_dwPower = mat.m_dwPower;
	m_dwColorId = mat.m_dwColorId;
	m_dwInternalId = mat.m_dwInternalId;
	m_dwInternalTexCoordsId = mat.m_dwInternalTexCoordsId;
	m_dwParent = mat.m_dwParent;
	m_dwTextureId = mat.m_dwTextureId;
	m_dwMaskId = mat.m_dwMaskId;
	m_dwEnvId = mat.m_dwEnvId;
	someColor = mat.someColor;
	m_fMoveX = mat.m_fMoveX;
	m_fMoveY = mat.m_fMoveY;
	m_fTransparency = mat.m_fTransparency;
	m_fSpecular = mat.m_fSpecular;
	m_fEnv1 = mat.m_fEnv1;
	m_fEnv2 = mat.m_fEnv2;
	m_fReflection = mat.m_fReflection;
	m_fRotation = mat.m_fRotation;
	m_fRotationPointX = mat.m_fRotationPointX;
	m_fRotationPointY = mat.m_fRotationPointY;
	m_pTreeItem = mat.m_pTreeItem;
	m_pGameModule = mat.m_pGameModule;
}

Material::~Material()
{

}

void Material::Load(FILE *f)
{
	char *buf;
	buf = FileManager::ReadStringZ(f);
	char *buf2 = buf;
	while (*buf2)
	{
		if (*buf2 == '"')
		{
			*buf2 = ' ';
		}
		buf2++;
	}
	m_sFullName = buf;
	m_sName = strtok(buf, " ");
	bool hasColor = false;
	while (true)
	{
		char *param = strtok(0, " ");
		if (!param)
			break;

		if (!strcmp(param, "col"))
		{
			m_dwColorId = atoi(strtok(0, " ")) - 1;
			hasColor = true;
		}
		else if (!strcmp(param, "tex"))
		{
			m_dwFlags |= MATERIAL_HAS_TEXTURE;
			m_dwTextureId = atoi(strtok(0, " ")) - 1;
		}
		else if (!strcmp(param, "ttx"))
		{
			m_dwFlags |= MATERIAL_HAS_TTX | MATERIAL_HAS_TEXTURE;
			m_dwTextureId = atoi(strtok(0, " ")) - 1;
		}
		else if (!strcmp(param, "itx"))
		{
			m_dwFlags |= MATERIAL_HAS_ITX | MATERIAL_HAS_TEXTURE;
			m_dwTextureId = atoi(strtok(0, " ")) - 1;
		}
		else if (!strcmp(param, "msk"))
		{
			m_dwFlags |= MATERIAL_HAS_MASK;
			m_dwMaskId = atoi(strtok(0, " ")) - 1;
		}
		else if (!strcmp(param, "transp"))
		{
			m_dwFlags |= MATERIAL_IS_TRANSPARENT;
			m_fTransparency = atof(strtok(0, " "));
		}
		else if (!strcmp(param, "specular"))
		{
			m_dwFlags |= MATERIAL_SPECULAR;
			char *spec = strtok(0, " ");
			if (spec)
				m_fSpecular = atof(spec);
		}
		else if (!strcmp(param, "reflect"))
		{
			char *refl = strtok(0, " ");
			if (refl)
				m_fReflection = atof(refl);
			m_dwFlags |= MATERIAL_HAS_REFLECTION;
		}
		else if (!strcmp(param, "alphamirr"))
		{
			m_dwFlags |= MATERIAL_HAS_ALPHAMIR;
		}
		else if (!strcmp(param, "wave"))
		{
			m_dwFlags |= MATERIAL_HAS_WAVE;
		}
		else if (!strcmp(param, "bumpcoord"))
		{
			m_dwFlags |= MATERIAL_HAS_BUMP;
		}
		else if (!strcmp(param, "power"))
		{
			m_dwFlags |= MATERIAL_POWER;
			char *power = strtok(0, " ");
			if (power)
				m_dwPower = atoi(power);
		}
		else if (!strcmp(param, "usecol"))
		{
			m_dwFlags |= MATERIAL_USE_COL;
		}
		else if (!strcmp(param, "env"))
		{
			char *env1s = strtok(0, " ");
			if (env1s)
				m_fEnv1 = atof(env1s);

			char *env2s = strtok(0, " ");
			if (env2s)
				m_fEnv2 = atof(env2s);

			m_dwEnvId = 0;
			m_dwFlags |= MATERIAL_ENV;
		}
		else if (!strncmp(param, "env", 3))
		{
			if (param[3])
				m_dwEnvId = param[3] - '0';
			else
				m_dwEnvId = 0;

			char *env1s = strtok(0, " ");
			if (env1s)
				m_fEnv1 = atof(env1s);

			char *env2s = strtok(0, " ");
			if (env2s)
				m_fEnv2 = atof(env2s);

			m_dwFlags |= MATERIAL_ENV;
		}
		else if (!strcmp(param, "coord"))
		{
			char *coord = strtok(0, " ");
			if (coord)
				m_dwInternalTexCoordsId = atoi(coord) - 1;
		}
		else if (!strcmp(param, "att"))
		{
			char *att = strtok(0, " ");
			if (att)
				m_dwInternalId = atoi(att) - 1;

			m_dwFlags |= MATERIAL_ATT;
		}
		else if (!strcmp(param, "par"))
		{
			char *par = strtok(0, " ");
			if (par)
				m_dwParent = atoi(par) - 1;
		}
		else if (!strcmp(param, "move"))
		{
			m_dwFlags |= MATERIAL_MOVE;
			m_fMoveX = atof(strtok(0, " "));
			m_fMoveY = atof(strtok(0, " "));
		}
		else if (!strcmp(param, "RotPoint"))
		{
			m_dwFlags |= MATERIAL_HAS_ROTATION_POINT;
			m_fRotationPointX = atof(strtok(0, " "));
			m_fRotationPointY = atof(strtok(0, " "));
		}
		else if (!strcmp(param, "rot"))
		{
			m_dwFlags |= MATERIAL_HAS_ROTATION;
			m_fRotation = atof(strtok(0, " "));
		}
		else if (!strcmp(param, "noz"))
		{
			m_dwFlags |= MATERIAL_NOZ;
		}
		else if (!strcmp(param, "nof"))
		{
			m_dwFlags |= MATERIAL_NOF;
		}
		else if (!strcmp(param, "notile"))
		{
			m_dwFlags |= MATERIAL_NOTILE;
		}
		else if (!strcmp(param, "notileu"))
		{
			m_dwFlags |= MATERIAL_NOTILEU;
		}
		else if (!strcmp(param, "notilev"))
		{
			m_dwFlags |= MATERIAL_NOTILEV;
		}
	}
	if (hasColor && !(m_dwFlags & MATERIAL_HAS_TTX))
	{
		m_dwFlags |= MATERIAL_USE_COL;
	}
}

Material &Material::operator=(Material &mat) 
{
	m_sFullName = mat.m_sFullName;
	m_sName = mat.m_sName;
	m_dwFlags = mat.m_dwFlags;
	m_dwPower = mat.m_dwPower;
	m_dwColorId = mat.m_dwColorId;
	m_dwInternalId = mat.m_dwInternalId;
	m_dwInternalTexCoordsId = mat.m_dwInternalTexCoordsId;
	m_dwParent = mat.m_dwParent;
	m_dwTextureId = mat.m_dwTextureId;
	m_dwMaskId = mat.m_dwMaskId;
	m_dwEnvId = mat.m_dwEnvId;
	someColor = mat.someColor;
	m_fMoveX = mat.m_fMoveX;
	m_fMoveY = mat.m_fMoveY;
	m_fTransparency = mat.m_fTransparency;
	m_fSpecular = mat.m_fSpecular;
	m_fEnv1 = mat.m_fEnv1;
	m_fEnv2 = mat.m_fEnv2;
	m_fReflection = mat.m_fReflection;
	m_fRotation = mat.m_fRotation;
	m_fRotationPointX = mat.m_fRotationPointX;
	m_fRotationPointY = mat.m_fRotationPointY;
	m_pTreeItem = mat.m_pTreeItem;
	m_pGameModule = mat.m_pGameModule;
	return *this;
}

bool Material::Save(FILE *f)
{
	m_sFullName = m_sName + " ";
	if (m_dwFlags & MATERIAL_USE_COL || m_dwFlags & MATERIAL_HAS_TTX)
	{
		m_sFullName += "col " + std::to_string(m_dwColorId + 1) + " ";
		if (m_dwFlags & MATERIAL_HAS_TEXTURE)
		{
			if (m_dwFlags & MATERIAL_HAS_TTX)
			{
				m_sFullName += "ttx " + std::to_string(m_dwTextureId + 1) + " ";
			}
			else
			{
				m_sFullName += "usecol tex " + std::to_string(m_dwTextureId + 1) + " ";
			}
		}
	}
	else if (m_dwFlags & MATERIAL_HAS_TEXTURE)
	{
		m_sFullName += "tex " + std::to_string(m_dwTextureId + 1) + " ";
	}
	if (m_dwFlags & MATERIAL_IS_TRANSPARENT)
		m_sFullName += "transp " + std::to_string(m_fTransparency) + " ";
	
	if (m_dwFlags & MATERIAL_HAS_ROTATION)
		m_sFullName += "rot " + std::to_string(m_fRotation) + " ";

	if (m_dwFlags & MATERIAL_HAS_ROTATION_POINT)
		m_sFullName += "RotPoint " + std::to_string(m_fRotationPointX) + " " + std::to_string(m_fRotationPointX) + " ";

	if (m_dwFlags & MATERIAL_MOVE)
		m_sFullName += "move " + std::to_string(m_fMoveX) + " " + std::to_string(m_fMoveY) + " ";

	if (m_dwFlags & MATERIAL_POWER)
		m_sFullName += "power " + std::to_string(m_dwPower) + " ";

	if (m_dwFlags & MATERIAL_SPECULAR)
		m_sFullName += "specular " + std::to_string(m_fSpecular) + " ";

	if (m_dwInternalId >= 0)
		m_sFullName += "att " + std::to_string(m_dwInternalId + 1) + " ";

	if (m_dwParent >= 0)
		m_sFullName += "par " + std::to_string(m_dwParent + 1) + " ";

	if (m_dwInternalTexCoordsId > 0)
		m_sFullName += "coord " + std::to_string(m_dwInternalTexCoordsId + 1) + " ";

	if (m_dwFlags & MATERIAL_NOZ)
		m_sFullName += "noz ";

	if (m_dwFlags & MATERIAL_NOF)
		m_sFullName += "nof ";

	if (m_dwFlags & MATERIAL_NOTILE)
		m_sFullName += "notile ";

	if (m_dwFlags & MATERIAL_ENV)
	{
		if (m_dwEnvId == 0)
			m_sFullName += "env " + std::to_string(m_fEnv1) + " " + std::to_string(m_fEnv2) + " ";
		else
			m_sFullName += "env" + std::to_string(m_dwEnvId) + " " + std::to_string(m_fEnv1) + " " + std::to_string(m_fEnv2) + " ";
	}
	
	FileManager::WriteStringZ(f, m_sFullName.c_str());
	return true;
}

void Material::SetTreeItem(class WinTreeItem *item)
{
	m_pTreeItem = item;
}

class WinTreeItem *Material::GetTreeItem()
{
	return m_pTreeItem;
}

void Material::SetGameModule(class GameModule *item)
{
	m_pGameModule = item;
}

class GameModule *Material::GetGameModule()
{
	return m_pGameModule;
}

void Material::Reset()
{
	m_dwFlags = 0;
	m_dwPower = 0;
	m_dwColorId = 0;
	m_dwInternalId = -1;
	m_dwInternalTexCoordsId = 0;
	m_dwParent = -1;
	m_dwTextureId = -1;
	m_dwMaskId = 0;
	m_dwEnvId = 0;
	someColor = 0;
	m_fMoveX = 0.f;
	m_fMoveY = 0.f;
	m_fTransparency = 0.f;
	m_fSpecular = 0.f;
	m_fEnv1 = 0.f;
	m_fEnv2 = 0.f;
	m_fReflection = 0.f;
	m_fRotation = 0.f;
	m_fRotationPointX = 0.f;
	m_fRotationPointY = 0.f;
}
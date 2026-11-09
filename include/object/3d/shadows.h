#pragma once

class CShadow
{
public:
	CShadow();
	virtual ~CShadow();

	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

private:
};

class CObjectX;

class CShadowObjectX : public CShadow
{
public:
	CShadowObjectX();
	~CShadowObjectX();


};
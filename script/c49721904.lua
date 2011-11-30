--真六武衆－キザン
function c49721904.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c49721904.spcon)
	c:RegisterEffect(e1)
	--atk,def
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c49721904.valcon)
	e2:SetValue(300)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e3)
end
function c49721904.spfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x3d) and c:GetCode()~=49721904
end
function c49721904.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0 and
		Duel.IsExistingMatchingCard(c49721904.spfilter,c:GetControler(),LOCATION_MZONE,0,1,nil)
end
function c49721904.vfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x3d)
end
function c49721904.valcon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c49721904.vfilter,c:GetControler(),LOCATION_MZONE,0,2,c)
end

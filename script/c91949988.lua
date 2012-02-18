--迅雷の騎士ガイアドラグーン
function c91949988.initial_effect(c)
	--xyz summon
	c:EnableReviveLimit()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_EXTRA)
	e1:SetCondition(c91949988.xyzcon)
	e1:SetOperation(c91949988.xyzop)
	e1:SetValue(SUMMON_TYPE_XYZ)
	c:RegisterEffect(e1)
	--pierce
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_PIERCE)
	c:RegisterEffect(e2)
end
function c91949988.xyzfilter(c)
	return c:GetLevel()==7
end
function c91949988.ovfilter(c)
	local rk=c:GetRank()
	return c:IsFaceup() and (rk==5 or rk==6)
end
function c91949988.xyzcon(e,c)
	if c==nil then return true end
	if Duel.IsExistingMatchingCard(c91949988.ovfilter,c:GetControler(),LOCATION_MZONE,0,1,nil) then return true end
	local g=Duel.GetXyzMaterial(c)
	return g:IsExists(c91949988.xyzfilter,2,nil)
end
function c91949988.xyzop(e,tp,eg,ep,ev,re,r,rp,c)
	local g=Duel.GetXyzMaterial(c)
	local b1=g:IsExists(c91949988.xyzfilter,2,nil)
	local b2=Duel.IsExistingMatchingCard(c91949988.ovfilter,tp,LOCATION_MZONE,0,1,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
	if (b1 and b2 and Duel.SelectYesNo(tp,aux.Stringid(91949988,0))) or ((not b1) and b2) then
		local mg=Duel.SelectMatchingCard(tp,c91949988.ovfilter,tp,LOCATION_MZONE,0,1,1,nil)
		local mg2=mg:GetFirst():GetOverlayGroup()
		Duel.Overlay(c,mg2)
		Duel.Overlay(c,mg)
		mg:Merge(mg2)
		c:SetMaterial(mg)
	else
		local mg=g:FilterSelect(tp,c91949988.xyzfilter,2,2,nil)
		Duel.Overlay(c,mg)
		c:SetMaterial(mg)
	end
end

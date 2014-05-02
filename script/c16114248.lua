--ペア・サイクロイド
function c16114248.initial_effect(c)
	c:EnableReviveLimit()
	--fusion material
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(c16114248.fscondition)
	e1:SetOperation(c16114248.fsoperation)
	c:RegisterEffect(e1)
	--direct attack
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_DIRECT_ATTACK)
	c:RegisterEffect(e2)
end
function c16114248.spfilter(c,mg)
	return c:IsRace(RACE_MACHINE) and mg:IsExists(c16114248.spfilter2,1,c,c:GetCode())
end
function c16114248.spfilter2(c,code)
	return c:IsRace(RACE_MACHINE) and c:IsCode(code)
end
function c16114248.fscondition(e,mg,gc)
	if mg==nil then return true end
	if gc then return false end
	return mg:IsExists(c16114248.spfilter,1,nil,mg)
end
function c16114248.fsoperation(e,tp,eg,ep,ev,re,r,rp,gc)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	local g1=eg:FilterSelect(tp,c16114248.spfilter,1,1,nil,eg)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	local g2=eg:FilterSelect(tp,c16114248.spfilter2,1,1,g1:GetFirst(),g1:GetFirst():GetCode())
	g1:Merge(g2)
	Duel.SetFusionMaterial(g1)
end
--光の創造神 ホルアクティ
function c10000040.initial_effect(c)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c10000040.spcon)
	e1:SetOperation(c10000040.spop)
	c:RegisterEffect(e1)
	--spsummon condition
	local e2=Effect.CreateEffect(c)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e2)
	--spsummon
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_CANNOT_DISABLE_SPSUMMON)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	c:RegisterEffect(e3)
end
function c10000040.spfilter(c,code)
	return c:GetOriginalCode()==code
end
function c10000040.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>-3
		and Duel.CheckReleaseGroup(tp,c10000040.spfilter,1,nil,10000000)
		and Duel.CheckReleaseGroup(tp,c10000040.spfilter,1,nil,10000010)
		and Duel.CheckReleaseGroup(tp,c10000040.spfilter,1,nil,10000020)
end
function c10000040.spop(e,tp,eg,ep,ev,re,r,rp,c)
	local g1=Duel.SelectReleaseGroup(tp,c10000040.spfilter,1,1,nil,10000000)
	local g2=Duel.SelectReleaseGroup(tp,c10000040.spfilter,1,1,nil,10000010)
	local g3=Duel.SelectReleaseGroup(tp,c10000040.spfilter,1,1,nil,10000020)
	g1:Merge(g2)
	g1:Merge(g3)
	Duel.Release(g1,REASON_COST)
	local WIN_REASON_CREATORGOD = 0x13
	Duel.Win(tp,WIN_REASON_CREATORGOD)
end

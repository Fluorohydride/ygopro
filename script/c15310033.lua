--アンノウン·シンクロン
function c15310033.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c15310033.spcon)
	e1:SetOperation(c15310033.spop)
	c:RegisterEffect(e1)
end
c15310033[0]=true
c15310033[1]=true
function c15310033.spcon(e,c)
	if c==nil then return true end
	return c15310033[c:GetControler()] and Duel.GetFieldGroupCount(c:GetControler(),LOCATION_MZONE,0,nil)==0
		and	Duel.GetFieldGroupCount(c:GetControler(),0,LOCATION_MZONE,nil)>0
		and Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
end
function c15310033.spop(e,tp,eg,ep,ev,re,r,rp,c)
	c15310033[tp]=false
end

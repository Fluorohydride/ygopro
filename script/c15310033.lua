--アンノウン・シンクロン
function c15310033.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCountLimit(1,15310033+EFFECT_COUNT_CODE_DUEL)
	e1:SetCondition(c15310033.spcon)
	c:RegisterEffect(e1)
end
function c15310033.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0,nil)==0
		and	Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE,nil)>0
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
end

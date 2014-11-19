--先史遺産トゥーラ・ガーディアン
function c5291803.initial_effect(c)
	c:SetUniqueOnField(1,0,5291803)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c5291803.spcon)
	c:RegisterEffect(e1)
end
function c5291803.cfilter(tc)
	return tc and tc:IsFaceup()
end
function c5291803.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and (c5291803.cfilter(Duel.GetFieldCard(tp,LOCATION_SZONE,5)) or c5291803.cfilter(Duel.GetFieldCard(1-tp,LOCATION_SZONE,5)))
end

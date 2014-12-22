--シンクロン・キャリアー
function c62125438.initial_effect(c)
	--extra summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_EXTRA_SUMMON_COUNT)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_HAND+LOCATION_MZONE,0)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsSetCard,0x17))
	c:RegisterEffect(e1)
	--search
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOKEN)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetProperty(EFFECT_FLAG_DELAY)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EVENT_BE_MATERIAL)
	e2:SetCountLimit(1,62125438)
	e2:SetCondition(c62125438.spcon)
	e2:SetTarget(c62125438.sptg)
	e2:SetOperation(c62125438.spop)
	c:RegisterEffect(e2)
end
function c62125438.filter(c,tp)
	return c:IsSetCard(0x17) and c:IsType(TYPE_MONSTER) and c:IsControler(tp) and c:IsLocation(LOCATION_GRAVE)
end
function c62125438.spcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c62125438.filter,1,nil,tp) and r==REASON_SYNCHRO and re:GetHandler():IsRace(RACE_WARRIOR+RACE_MACHINE)
end
function c62125438.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsPlayerCanSpecialSummonMonster(tp,62125439,0x17,0x4011,1000,0,2,RACE_MACHINE,ATTRIBUTE_EARTH) end
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,0)
end
function c62125438.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0
		or not Duel.IsPlayerCanSpecialSummonMonster(tp,62125439,0x17,0x4011,1000,0,2,RACE_MACHINE,ATTRIBUTE_EARTH) then return end
	local token=Duel.CreateToken(tp,62125439)
	Duel.SpecialSummon(token,0,tp,tp,false,false,POS_FACEUP)
end

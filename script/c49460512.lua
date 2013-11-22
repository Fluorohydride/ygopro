--BF－流離いのコガラシ
function c49460512.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(49460512,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c49460512.spcon)
	e1:SetTarget(c49460512.sptg)
	e1:SetOperation(c49460512.spop)
	c:RegisterEffect(e1)
	--activate limit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_BE_MATERIAL)
	e2:SetCondition(c49460512.limitcon)
	e2:SetOperation(c49460512.limitop)
	c:RegisterEffect(e2)
end
function c49460512.cfilter(c,tp)
	return c:IsSetCard(0x33) and c:GetPreviousControler()==tp and c:IsPreviousLocation(LOCATION_MZONE)
		and c:IsPreviousPosition(POS_FACEUP) and bit.band(c:GetReason(),0x41)==0x41
end
function c49460512.spcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c49460512.cfilter,1,nil,tp)
end
function c49460512.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c49460512.spop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
	end
end
function c49460512.limitcon(e,tp,eg,ep,ev,re,r,rp)
	return r==REASON_SYNCHRO
end
function c49460512.limitop(e,tp,eg,ep,ev,re,r,rp)
	Duel.SetChainLimitTillChainEnd(c49460512.chainlm)
end
function c49460512.chainlm(e,rp,tp)
	return tp==rp
end

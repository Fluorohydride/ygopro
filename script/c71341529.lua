--インフェルニティ・ナイト
function c71341529.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(71341529,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_CVAL_CHECK)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c71341529.spcon)
	e1:SetCost(c71341529.spcost)
	e1:SetTarget(c71341529.sptg)
	e1:SetOperation(c71341529.spop)
	e1:SetValue(c71341529.valcheck)
	c:RegisterEffect(e1)
end
function c71341529.spcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:IsReason(REASON_DESTROY) and bit.band(c:GetPreviousLocation(),LOCATION_ONFIELD)~=0
end
function c71341529.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if Duel.GetFlagEffect(tp,71341529)==0 then
			Duel.RegisterFlagEffect(tp,71341529,RESET_CHAIN,0,1)
			c71341529[0]=Duel.GetMatchingGroupCount(Card.IsDiscardable,tp,LOCATION_HAND,0,e:GetHandler())
			c71341529[1]=0
		end
		return c71341529[0]-c71341529[1]>=2
	end
	Duel.DiscardHand(tp,Card.IsDiscardable,2,2,REASON_COST+REASON_DISCARD)
end
function c71341529.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c71341529.spop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
	end
end
function c71341529.valcheck(e)
	c71341529[1]=c71341529[1]+2
end

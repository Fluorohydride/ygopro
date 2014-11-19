--フィッシュボーグ－アーチャー
function c62023839.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(62023839,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCountLimit(1,62023839)
	e1:SetCondition(c62023839.spcon)
	e1:SetCost(c62023839.spcost)
	e1:SetTarget(c62023839.sptg)
	e1:SetOperation(c62023839.spop)
	c:RegisterEffect(e1)
end
function c62023839.spcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)==0
end
function c62023839.cfilter(c)
	return c:IsAttribute(ATTRIBUTE_WATER) and c:IsDiscardable()
end
function c62023839.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c62023839.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c62023839.cfilter,1,1,REASON_COST+REASON_DISCARD)
end
function c62023839.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c62023839.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)>0 then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EVENT_PHASE_START+PHASE_BATTLE)
		e1:SetCountLimit(1)
		e1:SetOperation(c62023839.desop)
		e1:SetReset(RESET_PHASE+RESET_END)
		Duel.RegisterEffect(e1,tp)
	end
end
function c62023839.desfilter(c)
	return c:IsFacedown() or not c:IsAttribute(ATTRIBUTE_WATER)
end
function c62023839.desop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c62023839.desfilter,tp,LOCATION_MZONE,0,nil)
	if g:GetCount()>0 then
		Duel.Hint(HINT_CARD,0,62023839)
		Duel.Destroy(g,REASON_EFFECT)
	end
end

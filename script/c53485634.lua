--真紅眼の遡刻竜
function c53485634.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(53485634,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c53485634.spcon)
	e1:SetTarget(c53485634.sptg)
	e1:SetOperation(c53485634.spop)
	c:RegisterEffect(e1)
	--extra summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(53485634,1))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCost(c53485634.sumcost)
	e2:SetOperation(c53485634.sumop)
	c:RegisterEffect(e2)
end
function c53485634.cfilter(c,tp)
	return c:IsLevelBelow(7) and c:IsSetCard(0x3b) and c:GetPreviousControler()==tp
		and c:IsPreviousPosition(POS_FACEUP) and c:IsPreviousLocation(LOCATION_MZONE)
		and c:IsReason(REASON_EFFECT+REASON_BATTLE) and c:GetReasonPlayer()~=tp and c:IsControler(tp)
end
function c53485634.spcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c53485634.cfilter,1,nil,tp)
end
function c53485634.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanSpecialSummonCount(tp,2)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c53485634.spfilter(c,e,tp)
	return c:IsCanBeSpecialSummoned(e,0,tp,false,false,c:GetPreviousPosition())
end
function c53485634.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP_DEFENCE)~=0 then
		local g=eg:Filter(c5724.spfilter,nil,e,tp)
		local ct=Duel.GetLocationCount(tp,LOCATION_MZONE)
		if g:GetCount()<=0 or ct<=0 then return end
		if g:GetCount()>ct then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
			g=g:Select(tp,ct,ct,nil)
		end
		local tc=g:GetFirst()
		while tc do
			Duel.SpecialSummonStep(tc,0,tp,tp,false,false,tc:GetPreviousPosition())
			tc=g:GetNext()
		end
		Duel.SpecialSummonComplete()
	end
end
function c53485634.sumcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c53485634.sumop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFlagEffect(tp,53485634)~=0 then return end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetTargetRange(LOCATION_HAND,0)
	e1:SetCode(EFFECT_EXTRA_SUMMON_COUNT)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsSetCard,0x3b))
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
	Duel.RegisterFlagEffect(tp,53485634,RESET_PHASE+PHASE_END,0,1)
end

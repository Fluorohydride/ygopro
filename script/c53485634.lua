--真紅眼の遡刻竜
function c53485634.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(53485634,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetRange(LOCATION_HAND)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e1:SetCondition(c53485634.spcon)
	e1:SetTarget(c53485634.sptg)
	e1:SetOperation(c53485634.spop)
	c:RegisterEffect(e1)
	--extra summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(53485634,1))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c53485634.sumcon)
	e2:SetCost(c53485634.sumcost)
	e2:SetTarget(c53485634.sumtg)
	e2:SetOperation(c53485634.sumop)
	c:RegisterEffect(e2)
end
function c53485634.spfilter(c,e,tp)
	return c:IsPreviousPosition(POS_FACEUP) and c:GetPreviousControler()==tp and c:IsPreviousLocation(LOCATION_MZONE)
		and c:IsReason(REASON_DESTROY) and (c:IsReason(REASON_EFFECT) or (c:IsReason(REASON_BATTLE) and Duel.GetAttacker():IsControler(1-tp)))
		and c:IsSetCard(0x3b) and c:IsLevelBelow(7) and c:IsControler(tp)
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c53485634.spcon(e,tp,eg,ep,ev,re,r,rp)
	return rp~=tp
end
function c53485634.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanSpecialSummonCount(tp,2)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false)
		and eg:IsExists(c53485634.spfilter,1,nil,e,tp) end
	local g=eg:Filter(c53485634.spfilter,nil,e,tp)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c53485634.filter(c,e,tp)
	return c:IsRelateToEffect(e) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c53485634.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(c53485634.filter,nil,e,tp)
	if Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP_DEFENCE)~=0 then
		local ct=Duel.GetLocationCount(tp,LOCATION_MZONE)
		if ct<=0 or g:GetCount()==0 then return end
		if g:GetCount()>ct then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
			g=g:Select(tp,ct,ct,nil)
		end
		local tc=g:GetFirst()
		while tc do
			local pos=tc:GetPreviousPosition()
			Duel.SpecialSummonStep(tc,0,tp,tp,false,false,pos)
			tc=g:GetNext()
		end
		Duel.SpecialSummonComplete()
	end
end
function c53485634.sumcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFlagEffect(tp,53485634)==0
end
function c53485634.sumcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c53485634.sumtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanSummon(tp) end
end
function c53485634.sumop(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_EXTRA_SUMMON_COUNT)
	e1:SetTargetRange(LOCATION_HAND+LOCATION_MZONE,0)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsSetCard,0x3b))
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
	Duel.RegisterFlagEffect(tp,53485634,RESET_PHASE+PHASE_END,0,1)
end

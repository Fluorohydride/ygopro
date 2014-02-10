--対峙するG
function c15721123.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(15721123,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetRange(LOCATION_HAND)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c15721123.spcon)
	e1:SetTarget(c15721123.sptg)
	e1:SetOperation(c15721123.spop)
	c:RegisterEffect(e1)
end
function c15721123.cfilter(c,tp)
	return c:GetSummonPlayer()==tp and c:IsPreviousLocation(LOCATION_EXTRA)
end
function c15721123.spcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c15721123.cfilter,1,nil,1-tp)
end
function c15721123.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c15721123.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)>0 then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetCode(EFFECT_IMMUNE_EFFECT)
		e1:SetValue(c15721123.efilter)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e1)
	end
end
function c15721123.efilter(e,te)
	if not te:IsActiveType(TYPE_MONSTER) then return false end
	local c=e:GetHandler()
	local ec=te:GetHandler()
	if ec:IsHasCardTarget(c) then return true end
	return te:IsHasType(EFFECT_TYPE_ACTIONS) and te:IsHasProperty(EFFECT_FLAG_CARD_TARGET) and c:IsRelateToEffect(te)
end

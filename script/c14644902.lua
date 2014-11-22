--幻想召喚師
function c14644902.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(14644902,0))
	e1:SetCategory(CATEGORY_RELEASE+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c14644902.target)
	e1:SetOperation(c14644902.operation)
	c:RegisterEffect(e1)
end
function c14644902.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c14644902.rfilter(c,e)
	return c:IsReleasableByEffect() and not c:IsImmuneToEffect(e)
end
function c14644902.filter(c,e,tp)
	return c:IsType(TYPE_FUSION) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c14644902.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local rg=Duel.SelectReleaseGroup(tp,c14644902.rfilter,1,1,e:GetHandler(),e)
	if Duel.Release(rg,REASON_EFFECT)>0 and Duel.GetLocationCount(tp,LOCATION_MZONE)>0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=Duel.SelectMatchingCard(tp,c14644902.filter,tp,LOCATION_EXTRA,0,1,1,nil,e,tp)
		if sg:GetCount()>0 then
			Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
			e1:SetRange(LOCATION_MZONE)
			e1:SetCode(EVENT_PHASE+PHASE_END)
			e1:SetCondition(c14644902.descon)
			e1:SetOperation(c14644902.desop)
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			e1:SetCountLimit(1)
			sg:GetFirst():RegisterEffect(e1)
		end
	end
end
function c14644902.descon(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():IsHasEffect(66235877)
end
function c14644902.desop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Destroy(e:GetHandler(),REASON_EFFECT)
end

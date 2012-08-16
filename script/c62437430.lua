--異次元海溝
function c62437430.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c62437430.target)
	e1:SetOperation(c62437430.operation)
	c:RegisterEffect(e1)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(62437430,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_LEAVE_FIELD)
	e2:SetCondition(c62437430.spcon)
	e2:SetTarget(c62437430.sptg)
	e2:SetOperation(c62437430.spop)
	c:RegisterEffect(e2)
	e1:SetLabelObject(e2)
end
function c62437430.filter(c)
	return c:IsAttribute(ATTRIBUTE_WATER) and c:IsAbleToRemove() and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
		and (not c:IsLocation(LOCATION_MZONE) or c:IsFaceup())
end
function c62437430.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c62437430.filter,tp,0x16,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,nil,1,tp,0x16)
end
function c62437430.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local tc=Duel.SelectMatchingCard(tp,c62437430.filter,tp,0x16,0,1,1,nil):GetFirst()
	if tc then
		Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)
		tc:RegisterFlagEffect(62437430,RESET_EVENT+0x1fe0000,0,0)
		e:GetLabelObject():SetLabelObject(tc)
	end
end
function c62437430.spcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=e:GetLabelObject()
	return tc and c:IsReason(REASON_DESTROY) and c:IsPreviousPosition(POS_FACEUP)
end
function c62437430.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetLabelObject():GetFlagEffect(62437430)~=0
		and e:GetLabelObject():GetReasonEffect():GetHandler()==e:GetHandler() end
	Duel.SetTargetCard(e:GetLabelObject())
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetLabelObject(),1,0,0)
end
function c62437430.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end

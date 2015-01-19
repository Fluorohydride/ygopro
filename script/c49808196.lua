--メタボ・サッカー
function c49808196.initial_effect(c)
	--token
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(49808196,1))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOKEN)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BE_MATERIAL)
	e1:SetCondition(c49808196.tkcon)
	e1:SetTarget(c49808196.tktg)
	e1:SetOperation(c49808196.tkop)
	c:RegisterEffect(e1)
end
function c49808196.tkcon(e,tp,eg,ep,ev,re,r,rp)
	return r==REASON_SUMMON and e:GetHandler():GetReasonCard():IsAttribute(ATTRIBUTE_DARK)
end
function c49808196.tktg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,3,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,3,0,0)
end
function c49808196.tkop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<3 then return end
	if not Duel.IsPlayerCanSpecialSummonMonster(tp,49808197,0,0x4011,0,0,1,RACE_FIEND,ATTRIBUTE_DARK) then return end
	for i=1,3 do
		local token=Duel.CreateToken(tp,49808197)
		Duel.SpecialSummonStep(token,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UNRELEASABLE_SUM)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetValue(1)
		token:RegisterEffect(e1,true)
	end
	Duel.SpecialSummonComplete()
end

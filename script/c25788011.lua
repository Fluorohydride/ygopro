--調星師ライズベルト
function c25788011.initial_effect(c)
	--lv up
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(25788011,0))
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetTarget(c25788011.target)
	e1:SetOperation(c25788011.operation)
	c:RegisterEffect(e1)
end
function c25788011.filter(c)
	return c:IsFaceup() and c:IsLevelAbove(1)
end
function c25788011.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c25788011.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c25788011.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c25788011.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
end
function c25788011.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local lv=Duel.SelectOption(tp,aux.Stringid(25788011,1),aux.Stringid(25788011,2),aux.Stringid(25788011,3))
		local e1=Effect.CreateEffect(c)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_LEVEL)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		e1:SetValue(lv+1)
		tc:RegisterEffect(e1)
	end
end

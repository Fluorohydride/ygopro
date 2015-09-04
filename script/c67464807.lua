--運命のドラ
function c67464807.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_ATTACK,0x1e0)
	e1:SetCondition(c67464807.condition)
	e1:SetTarget(c67464807.target)
	e1:SetOperation(c67464807.activate)
	c:RegisterEffect(e1)
end
function c67464807.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp
end
function c67464807.filter(c)
	return c:IsFaceup() and c:GetLevel()>1
end
function c67464807.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c67464807.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c67464807.filter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c67464807.filter,tp,0,LOCATION_MZONE,1,1,nil)
end
function c67464807.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local lv=tc:GetLevel()
		if lv>1 then
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
			e1:SetCode(EVENT_SUMMON_SUCCESS)
			e1:SetCountLimit(1)
			e1:SetCondition(c67464807.damcon)
			e1:SetOperation(c67464807.damop)
			e1:SetLabel(lv)
			e1:SetReset(RESET_PHASE+PHASE_END,2)
			Duel.RegisterEffect(e1,tp)
		end
	end
end
function c67464807.damcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp and ep==tp and eg:GetFirst():GetLevel()==e:GetLabel()-1
end
function c67464807.damop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Damage(1-tp,e:GetLabel()*500,REASON_EFFECT)
end

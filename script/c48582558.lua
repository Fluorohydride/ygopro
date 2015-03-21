--バウンサー・ガード
function c48582558.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c48582558.target)
	e1:SetOperation(c48582558.activate)
	c:RegisterEffect(e1)
end
function c48582558.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x6b)
end
function c48582558.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c48582558.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c48582558.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c48582558.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c48582558.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
		e1:SetValue(1)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
		e2:SetValue(aux.tgval)
		e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e2)
		local e3=Effect.CreateEffect(e:GetHandler())
		e3:SetType(EFFECT_TYPE_FIELD)
		e3:SetProperty(EFFECT_FLAG_SET_AVAILABLE+EFFECT_FLAG_IGNORE_IMMUNE)
		e3:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
		e3:SetTargetRange(LOCATION_MZONE,0)
		e3:SetCondition(c48582558.atkcon)
		e3:SetTarget(c48582558.atktg)
		e3:SetValue(aux.imval1)
		e3:SetReset(RESET_PHASE+PHASE_END)
		e3:SetLabelObject(tc)
		e3:SetLabel(tc:GetRealFieldID())
		Duel.RegisterEffect(e3,tp)
	end
end
function c48582558.atkcon(e)
	return e:GetLabelObject():GetRealFieldID()==e:GetLabel()
end
function c48582558.atktg(e,c)
	return c:GetRealFieldID()~=e:GetLabel()
end

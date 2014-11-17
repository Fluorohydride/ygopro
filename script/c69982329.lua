--ペンデュラム・ターン
function c69982329.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c69982329.target)
	e1:SetOperation(c69982329.activate)
	c:RegisterEffect(e1)
end
function c69982329.filter(c)
	return c:IsType(TYPE_PENDULUM)
end
function c69982329.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return c69982329.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c69982329.filter,tp,LOCATION_SZONE,LOCATION_SZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c69982329.filter,tp,LOCATION_SZONE,LOCATION_SZONE,1,1,nil)
	Duel.Hint(HINT_SELECTMSG,tp,565)
	local sc=Duel.AnnounceNumber(tp,1,2,3,4,5,6,7,8,9,10)
	e:SetLabel(sc)
end
function c69982329.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CHANGE_LSCALE)
		e1:SetValue(e:GetLabel())
		e1:SetReset(RESET_EVENT+0x1ff0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_CHANGE_RSCALE)
		tc:RegisterEffect(e2)
	end
end

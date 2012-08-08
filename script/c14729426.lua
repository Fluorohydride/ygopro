--惑星からの物体A
function c14729426.initial_effect(c)
	--return to hand
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(14729426,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_BATTLE_START)
	e1:SetOperation(c14729426.operation)
	c:RegisterEffect(e1)
end
function c14729426.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler()==Duel.GetAttackTarget() and e:GetHandler():IsAttackPos() then
		local a=Duel.GetAttacker()
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetDescription(aux.Stringid(14729426,0))
		e1:SetCategory(CATEGORY_CONTROL)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
		e1:SetCode(EVENT_PHASE+PHASE_BATTLE)
		e1:SetCountLimit(1)
		e1:SetTarget(c14729426.cttg)
		e1:SetOperation(c14729426.ctop)
		e1:SetLabelObject(a)
		e1:SetLabel(a:GetRealFieldID())
		e1:SetReset(RESET_PHASE+PHASE_BATTLE)
		Duel.RegisterEffect(e1,tp)
	end
end
function c14729426.cttg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local a=e:GetLabelObject()
	if a:IsControler(1-tp) and a:GetRealFieldID()==e:GetLabel() then
		Duel.SetTargetCard(a)
		Duel.SetOperationInfo(0,CATEGORY_CONTROL,a,1,0,0)
	end
end
function c14729426.ctop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) and not Duel.GetControl(tc,tp) then
		if not tc:IsImmuneToEffect(e) and tc:IsAbleToChangeControler() then
			Duel.Destroy(tc,REASON_EFFECT)
		end
	end
end

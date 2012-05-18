--星見鳥ラリス
function c41382147.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetCondition(c41382147.atkcon)
	e1:SetValue(c41382147.atkval)
	c:RegisterEffect(e1)
	--remove
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(41382147,0))
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCategory(CATEGORY_REMOVE)
	e2:SetCode(EVENT_DAMAGE_STEP_END)
	e2:SetCondition(c41382147.rmcon)
	e2:SetTarget(c41382147.rmtg)
	e2:SetOperation(c41382147.rmop)
	c:RegisterEffect(e2)
end
function c41382147.atkcon(e)
	local ph=Duel.GetCurrentPhase()
	return (ph==PHASE_DAMAGE or ph==PHASE_DAMAGE_CAL)
		and (Duel.GetAttacker()==e:GetHandler() or Duel.GetAttackTarget()==e:GetHandler()) and Duel.GetAttackTarget()~=nil
end
function c41382147.atkval(e,c)
	return e:GetHandler():GetBattleTarget():GetLevel()*200
end
function c41382147.rmcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:IsRelateToBattle() and c==Duel.GetAttacker() and c:IsFaceup()
end
function c41382147.rmtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,e:GetHandler(),1,0,0)
end
function c41382147.rmop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() and Duel.Remove(c,POS_FACEUP,REASON_EFFECT+REASON_TEMPORARY)==1 then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EVENT_PHASE_START+PHASE_BATTLE)
		e1:SetReset(RESET_PHASE+PHASE_END+RESET_SELF_TURN,2)
		e1:SetLabelObject(c)
		e1:SetCountLimit(1)
		e1:SetOperation(c41382147.retop)
		Duel.RegisterEffect(e1,tp)
	end
end
function c41382147.retop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetTurnPlayer()==tp then
		Duel.ReturnToField(e:GetLabelObject())
	end
end

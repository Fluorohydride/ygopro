--ロケット戦士
function c30860696.initial_effect(c)
	--invincible
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetCondition(c30860696.ivcon)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_AVOID_BATTLE_DAMAGE)
	e2:SetCondition(c30860696.ivcon)
	e2:SetValue(1)
	c:RegisterEffect(e2)
	--reduce atk
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(30860696,0))
	e3:SetCategory(CATEGORY_ATKCHANGE)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_BATTLED)
	e3:SetCondition(c30860696.racon)
	e3:SetOperation(c30860696.raop)
	c:RegisterEffect(e3)
end
function c30860696.ivcon(e)
	return Duel.GetTurnPlayer()==e:GetHandlerPlayer()
end
function c30860696.racon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler()==Duel.GetAttacker() and Duel.GetAttackTarget()
end
function c30860696.raop(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.GetAttackTarget()
	if not d:IsRelateToBattle() or d:IsFacedown() then return end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(-500)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	d:RegisterEffect(e1)
end
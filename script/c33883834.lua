--紫炎の寄子
function c33883834.initial_effect(c)
	--indestructable
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(33883834,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_QUICK_O+EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_HAND)
	e1:SetCode(EVENT_PRE_DAMAGE_CALCULATE)
	e1:SetCondition(c33883834.con)
	e1:SetCost(c33883834.cost)
	e1:SetOperation(c33883834.op)
	c:RegisterEffect(e1)
end
function c33883834.con(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	return d and ((a:GetControler()==tp and a:IsSetCard(0x3d)) or (d:GetControler()==tp and d:IsSetCard(0x3d)))
		and Duel.GetFlagEffect(tp,33883834)==0
end
function c33883834.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
	Duel.RegisterFlagEffect(tp,33883834,RESET_PHASE+RESET_DAMAGE_CAL,0,1)
end
function c33883834.op(e,tp,eg,ep,ev,re,r,rp,chk)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	if not a:IsRelateToBattle() or not d:IsRelateToBattle() then return end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetOwnerPlayer(tp)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	e1:SetValue(1)
	if a:GetControler()==tp then
		a:RegisterEffect(e1)
	else
		d:RegisterEffect(e1)
	end
end

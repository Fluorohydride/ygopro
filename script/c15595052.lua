--封魔の伝承者
function c15595052.initial_effect(c)
	--to defence
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(15595052,0))
	e1:SetType(EFFECT_TYPE_TRIGGER_F+EFFECT_TYPE_SINGLE)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetOperation(c15595052.ancop)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
	c:RegisterEffect(e2)
	local e3=e1:Clone()
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e3)
end
function c15595052.ancop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local ct=Duel.GetMatchingGroupCount(Card.IsCode,tp,LOCATION_GRAVE,0,nil,15595052)
	if ct>0 and c:IsFaceup() and c:IsRelateToEffect(e) then
		Duel.Hint(HINT_SELECTMSG,tp,562)
		local att=Duel.AnnounceAttribute(tp,ct,0xffff)
		e:GetHandler():SetHint(CHINT_ATTRIBUTE,att)
		--destroy
		local e1=Effect.CreateEffect(c)
		e1:SetDescription(aux.Stringid(15595052,1))
		e1:SetCategory(CATEGORY_DESTROY)
		e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
		e1:SetCode(EVENT_BATTLE_START)
		e1:SetTarget(c15595052.destg)
		e1:SetOperation(c15595052.desop)
		e1:SetLabel(att)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
	end
end
function c15595052.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	local bc=Duel.GetAttackTarget()
	if chk==0 then return c==Duel.GetAttacker() and bc and bc:IsFaceup() and bc:IsAttribute(e:GetLabel()) end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,bc,1,0,0)
end
function c15595052.desop(e,tp,eg,ep,ev,re,r,rp)
	local bc=Duel.GetAttackTarget()
	if bc:IsRelateToBattle() and bc:IsFaceup() then
		Duel.Destroy(bc,REASON_EFFECT)
	end
end

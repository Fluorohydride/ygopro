--７つの武器を持つハンター
function c1525329.initial_effect(c)
	--arm
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(1525329,0))
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTarget(c1525329.armtg)
	e1:SetOperation(c1525329.armop)
	c:RegisterEffect(e1)
end
function c1525329.armtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,563)
	local rc=Duel.AnnounceRace(tp,1,0xffffff)
	e:SetLabel(rc)
end
function c1525329.armop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		local rc=e:GetLabel()
		e:GetHandler():SetHint(CHINT_RACE,rc)
		--atk up
		local e1=Effect.CreateEffect(c)
		e1:SetDescription(aux.Stringid(1525329,1))
		e1:SetCategory(CATEGORY_ATKCHANGE)
		e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
		e1:SetCode(EVENT_PRE_DAMAGE_CALCULATE)
		e1:SetCondition(c1525329.upcon)
		e1:SetOperation(c1525329.upop)
		e1:SetLabel(rc)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e1)
	end
end
function c1525329.upcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	return bc and bc:IsRace(e:GetLabel())
end
function c1525329.upop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetReset(RESET_PHASE+RESET_DAMAGE_CAL)
		e1:SetValue(1000)
		c:RegisterEffect(e1)
	end
end

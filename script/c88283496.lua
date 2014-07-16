--極星獣ガルム
function c88283496.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(88283496,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLED)
	e1:SetCondition(c88283496.retcon)
	e1:SetTarget(c88283496.rettg)
	e1:SetOperation(c88283496.retop)
	c:RegisterEffect(e1)
end
function c88283496.retcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetAttacker()
	if tc==e:GetHandler() then tc=Duel.GetAttackTarget() end
	if not tc then return false end
	e:SetLabelObject(tc)
	local lv=tc:GetLevel()
	return lv>0 and lv<=4 and not tc:IsStatus(STATUS_BATTLE_DESTROYED)
end
function c88283496.rettg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk ==0 then	return e:GetLabelObject():IsAbleToHand() end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,e:GetLabelObject(),0,0,0)
end
function c88283496.retop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabelObject():IsRelateToBattle() then
		Duel.SendtoHand(e:GetLabelObject(),nil,REASON_EFFECT)
	end
end

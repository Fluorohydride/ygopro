--幻層の守護者アルマデス
function c88033975.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,nil,aux.NonTuner(nil),1)
	c:EnableReviveLimit()
	--actlimit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	e1:SetCode(EFFECT_CANNOT_TRIGGER)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,0xff)
	e1:SetCondition(c88033975.actcon)
	e1:SetTarget(c88033975.acttg)
	c:RegisterEffect(e1)
end
function c88033975.actcon(e)
	return Duel.GetAttacker()==e:GetHandler() or Duel.GetAttackTarget()==e:GetHandler()
end
function c88033975.acttg(e,c)
	local tp=e:GetHandler():GetControler()
	return not (c:IsLocation(LOCATION_REMOVED) and c:IsReason(REASON_TEMPORARY) and c:GetPreviousControler()==tp)
end

--E・HERO エリクシーラー
function c29343734.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcCode4(c,21844576,58932615,84327329,79979666,true,true)
	--spsummon condition
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetCode(EFFECT_SPSUMMON_CONDITION)
	e2:SetValue(c29343734.splimit)
	c:RegisterEffect(e2)
	--attribute
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EFFECT_ADD_ATTRIBUTE)
	e3:SetValue(0xf)
	c:RegisterEffect(e3)
	--return
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(29343734,0))
	e4:SetCategory(CATEGORY_TODECK)
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e4:SetCode(EVENT_SPSUMMON_SUCCESS)
	e4:SetCondition(c29343734.retcon)
	e4:SetTarget(c29343734.rettg)
	e4:SetOperation(c29343734.retop)
	c:RegisterEffect(e4)
	--atk
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e5:SetRange(LOCATION_MZONE)
	e5:SetCode(EFFECT_UPDATE_ATTACK)
	e5:SetValue(c29343734.val)
	c:RegisterEffect(e5)
end
function c29343734.splimit(e,se,sp,st)
	return bit.band(st,SUMMON_TYPE_FUSION)==SUMMON_TYPE_FUSION
end
function c29343734.retcon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(e:GetHandler():GetSummonType(),SUMMON_TYPE_FUSION)==SUMMON_TYPE_FUSION
end
function c29343734.rettg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local g=Duel.GetMatchingGroup(Card.IsAbleToDeck,tp,LOCATION_REMOVED,LOCATION_REMOVED,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,g:GetCount(),0,0)
end
function c29343734.retop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(nil,tp,LOCATION_REMOVED,LOCATION_REMOVED,nil)
	Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
end
function c29343734.atkfilter(c,att)
	return c:IsFaceup() and c:IsAttribute(att)
end
function c29343734.val(e,c)
	return Duel.GetMatchingGroupCount(c29343734.atkfilter,c:GetControler(),0,LOCATION_MZONE,nil,c:GetAttribute())*300
end
